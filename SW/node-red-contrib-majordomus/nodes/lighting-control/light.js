module.exports = function(RED) {
    function LightNode(config) {
        RED.nodes.createNode(this, config);
        let node = this;
        
        node.name = config.name;
        node.typeLight = config.typeLight;
        node.rampTime = node.typeLight === 'onoff' ? 0 : parseInt(config.rampTime, 10) || 1000;
        node.autoOnMotion = config.autoOnMotion === "true";
        node.autoOnNight = config.autoOnNight === "true";
        node.nightThreshold = parseInt(config.nightThreshold, 10) || 50;
        node.autoOffNoMotion = config.autoOffNoMotion === "true";
        node.offDelay = parseInt(config.offDelay, 10) || 60; // Default timeout 60s

        node.context().set('state', 0)
        node.context().set('intensity', 0)
        node.context().set('presense', 0)
        
        let timeout = null;
        node.roomLightIntensity = 0
        node.outputMaxIntensity = 100

        function rampIntensity(target, duration, send) {
            let stepTime = 100; // Interval step time in ms
            let intensity = node.context().get('intensity')
            let diff = target - intensity // in %
            let computedDuration = duration * (Math.abs(diff)/100)
            let steps = (computedDuration / stepTime) <= 0 ? 1 : (computedDuration / stepTime) ;
            let stepSize = diff / steps;
            let currentStep = 1;
            let state = node.context().get('state')
  
            let rampInterval = setInterval(() => {
                
                intensity += stepSize;           

                if (currentStep >= steps) {
                    clearInterval(rampInterval);
                    intensity = target;
                    if(target === 0)
                    {
                        state = 0;
                    }
                    else {
                        state = 1;
                    }
                    send([{ topic: 'state', payload: state }, { topic: 'intensity', payload: intensity }]);
                }
                else {
                    if(state == 0)
                    {
                        state = 1
                        send([{ topic: 'state', payload: state }, null]);
                    }
                    send([null, { topic: 'intensity', payload: intensity }]);
                }

                // Update context
                node.context().set('state', state);
                node.context().set('intensity', intensity);

                // Update status
                let lastChanged = new Date().toLocaleString();
                node.context().set('lastChanged', lastChanged);
                let statusText = `State: ${state === 1 ? 'ON' : 'OFF'}, Intensity: ${intensity}%, Last changed: ${lastChanged}`;
                node.status({ fill: state === 1 ? "green" : "grey", shape: "dot", text: statusText });

                 
                currentStep++;
            }, stepTime);
        }

        function checkOnNoPresense(send) {
            if (node.autoOffNoMotion) {

                if (node.context().get('presense') == 1) {
                    if (node.timeout) {
                        clearTimeout(node.timeout);
                        node.timeout = null;
                    }
                } else if (node.context().get('presense') == 0) {
                    if (!node.timeout) {
                        node.timeout = setTimeout(() => {
                            rampIntensity(0, node.rampTime, send);
                            node.timeout = null;
                        }, node.offDelay * 1000);
                    }
                }
            }
        }

        node.on('input', function(msg, send, done) {
            let input = msg.payload;
            let state = node.context().get('state')
            let intensity = node.context().get('intensity')
            
            
            switch (msg.topic) {
                case 'button': // Toggle light on button press
                    if(input == 1) {
                        if(state == 1)
                        {
                            rampIntensity(0, node.rampTime, send);
                        }
                        else {
                            rampIntensity(node.outputMaxIntensity, node.rampTime, send);
                        }
                        checkOnNoPresense(send)
                    }
                    break;
                case 'setState': // set status
                    if(input == 1) {
                        rampIntensity(node.outputMaxIntensity, node.rampTime, send);
                    }
                    else {
                        rampIntensity(0, this.rampTime, send);
                    }
                    checkOnNoPresense(send)
                    break;
                case 'setIntensity': // set status
                    node.outputMaxIntensity = input
                    // update intensity if light is on
                    if(node.context().get('state') == 1)
                        {
                            rampIntensity(node.outputMaxIntensity, 0, send);
                        }
    
                    break;
                case 'presense': // Motion detected
                    node.context().set('presense', input)
                    if (input === 1 && node.autoOnMotion) {
                        if (!node.autoOnNight || (node.autoOnNight && node.nightThreshold > node.roomLightIntensity)) {
                            rampIntensity(node.outputMaxIntensity, node.rampTime, send);
                        }
                    }
                    checkOnNoPresense(send)    

                    break;
                case 'lightIntensity': // Adjust intensity based on light level
                    node.roomLightIntensity = input;
                    break;
                default:
                    node.warn(`Unknown topic: ${msg.topic}`);
            }

            if (done) done();
        });
    }

    RED.nodes.registerType("light", LightNode);
};
