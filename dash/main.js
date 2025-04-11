window.addEventListener("DOMContentLoaded", () => {
    // Open the WebSocket connection and register event handlers.
    const websocket = new WebSocket("ws://localhost:8769/");

    receiveMessage(websocket);
    sendMessage(websocket);

});

function sendMessage(websocket) {
    const form = document.querySelector("#userinfo");

    form.addEventListener("submit", async (event) => {
        const msg = document.querySelector("#username").value;
        const data = {id: 1, msg: msg};

        websocket.send(JSON.stringify(data));
    });
}

function receiveMessage(websocket) {
    websocket.addEventListener("message", ({ data }) => {
        console.log(data)
        // const event = data
        //
        // console.log(event)
    });
}




