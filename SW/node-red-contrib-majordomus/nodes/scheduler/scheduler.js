module.exports = function (RED) {
    function SchedulerNode(config) {
        RED.nodes.createNode(this, config);
        const node = this;

        // Initialize the schedule list
        node.scheduleList = config.scheduleList || [];

        // Timer interval in milliseconds
        const interval = 1000; // 1 second

        // Function to get the current value based on the schedule
        function evaluateSchedule() {
            const now = new Date();
            const currentDay = ["Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"][now.getDay()];
            const currentMinutes = now.getHours() * 60 + now.getMinutes();
        
            let currentValue = null;
            let currentRule = null
        
            for (let i = 0; i < node.scheduleList.length; i++) {
                const item = node.scheduleList[i];
                if (
                    item.text === currentDay ||
                    (item.text === "Monday-Friday" && ["Monday", "Tuesday", "Wednesday", "Thursday", "Friday"].includes(currentDay)) ||
                    (item.text === "Saturday-Sunday" && ["Saturday", "Sunday"].includes(currentDay)) ||
                    item.text === "Monday-Sunday"
                ) {
                    const startMinutes = parseTime(item.startTime);
                    const endMinutes = parseTime(item.endTime);
            
                    if (currentMinutes >= startMinutes && currentMinutes < endMinutes) {
                        if (item.type === 'bool') {
                            currentValue = item.value === 'true';
                        }
                        else if (item.type === 'num') {
                            currentValue = parseFloat(item.value);
                        }
                        else {
                            currentValue = item.value;
                        }
                        currentRule = item.text + " (" + item.startTime + " - " + item.endTime + ")";
                        break;
                    }
                }
            }
        
            if (currentValue !== null) {
                node.send({ payload: currentValue });
                node.status({ fill: "green", shape: "dot", text: `Current rule: ${currentRule}, Output: ${currentValue}` });
            } else {
                node.status({ fill: "grey", shape: "ring", text: ` No rule matches` });
            }
        }        

        // Helper function to parse time (HH:MM) into minutes
        function parseTime(time) {
            const [hours, minutes] = time.split(":").map(Number);
            return (hours || 0) * 60 + (minutes || 0);
        }

        // Start the timer
        const timer = setInterval(evaluateSchedule, interval);

        node.on('close', function (removed, done) {
            // Clear the timer when the node is closed
            clearInterval(timer);
            done();
        });
    }

    RED.nodes.registerType("scheduler", SchedulerNode, {
        defaults: {
            name: { value: "" },
            scheduleList: { value: [], required: false },
        },
    });
};
