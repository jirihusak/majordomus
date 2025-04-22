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
        
        let timeout = null;
        let roomLightIntensity = 0

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

        node.on('input', function(msg, send, done) {
            let input = msg.payload;
            let state = node.context().get('state')
            let intensity = node.context().get('intensity')
            
            
            switch (msg.topic) {
                case 'button': // Toggle light on button press
                    if(input == 1) {
                        if(state == 1)
                        {
                            rampIntensity(0, this.rampTime, send);
                        }
                        else {
                            rampIntensity(100, this.rampTime, send);
                        }
                    }
                    break;
                case 'presense': // Motion detected
                    if (input === 1 && node.autoOnMotion) {
                        if (!node.autoOnNight || (node.autoOnNight && node.nightThreshold > roomLightIntensity)) {
                            rampIntensity(100, this.rampTime, send);
                        }
                    }
                    if (node.autoOffNoMotion) {
                        if (input === 1) {
                            if (timeout) {
                                clearTimeout(timeout);
                                timeout = null;
                            }
                        } else if (input === 0) {
                            if (!timeout) {
                                timeout = setTimeout(() => {
                                    rampIntensity(0, this.rampTime, send);
                                }, node.offDelay * 1000);
                            }
                        }
                    }
                    break;
                case 'lightIntensity': // Adjust intensity based on light level
                    roomLightIntensity = input;

                    break;
                default:
                    node.warn(`Unknown topic: ${msg.topic}`);
            }

            if (done) done();
        });
    }

    RED.nodes.registerType("light", LightNode);
};
