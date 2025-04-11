window.addEventListener("DOMContentLoaded", () => {
    // Open the WebSocket connection and register event handlers.
    let id = "id" + Math.random().toString(16).slice(2)

    const websocket = new WebSocket("ws://localhost:8769/");

    websocket.addEventListener("error", (event) => {
        console.log("WebSocket error: ", event);
    });

    receiveMessage(websocket);
    sendMessage(websocket, id);

});

function sendMessage(websocket, id) {
    const form = document.querySelector("#sendMessage");

    form.addEventListener("click", async (event) => {
        const msg = document.querySelector("#messageText").value;
        if(!msg)
            return
        const data = {name: id, msg: msg};

        addNewMessage("chat-message-right mb-4", data)

        websocket.send(JSON.stringify(data));
    });
}

function receiveMessage(websocket) {
    websocket.addEventListener("message", ({ data }) => {
        const res = JSON.parse(data);
        addNewMessage("chat-message-left pb-4", res);
    });
}


function addNewMessage(classesType, data) {
    const subject = document.querySelector("#chat");

    let div = document.createElement("div");
    div.setAttribute('class', classesType);

    div.innerHTML = `
            <div>
            <img src="https://bootdey.com/img/Content/avatar/avatar1.png" class="rounded-circle mr-1" alt="Chris Wood" width="40" height="40">
            <div class="text-muted small text-nowrap mt-2">2:43 am</div>
            </div>
            <div class="flex-shrink-1 bg-light rounded py-2 px-3 mr-3">
            <div class="font-weight-bold mb-1">${data["name"]}</div>
            ${data["msg"]}
            </div>
        `

    subject.insertAdjacentElement(
        "beforeend",
        div,
    );
}

