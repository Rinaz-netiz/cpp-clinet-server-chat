window.addEventListener("DOMContentLoaded", () => {
    // Open the WebSocket connection and register event handlers.

    const websocket = new WebSocket("ws://localhost:8731/");

    websocket.addEventListener("error", (event) => {
        console.log("WebSocket error: ", event);
    });

    let allCookies = document.cookie;
    if(!allCookies)
        registerNewUser();


    let id = getCookie("user");

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
        document.querySelector("#messageText").value = "";
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

function registerNewUser() {
    let modalWindow = document.getElementById("authModal");
    let chatToHid = document.getElementById("chatBody");

    modalWindow.classList.remove("d-none");
    chatToHid.classList.add("toShadow");

    document.getElementById("modalButton").addEventListener("click", () => {
        let theName = document.getElementById("username");

        let oldMsg = document.querySelector("#username + strong");
        if (oldMsg) oldMsg.remove();


        if(!theName.value)
        {
            let usernameInput = document.getElementById("username");
            usernameInput.insertAdjacentHTML("afterend", "<strong style='color: #9C1A1C'>Fill out your name </strong>")
            return;
        }

        let english = /^[A-Za-z0-9]*$/;
        for(let i = 0; i < theName.value.length; i++)
            if(!english.test(theName.value[i]))
            {
                theName.value = "";
                let usernameInput = document.getElementById("username");
                usernameInput.insertAdjacentHTML("afterend", "<strong style='color: #9C1A1C'>Use only Latin characters.</strong>")
                return;
            }

        setCookie("user", theName.value)
        theName.value = "";

        modalWindow.classList.add("d-none");
        chatToHid.classList.remove("toShadow");
    })
}

// возвращает куки с указанным name,
// или undefined, если ничего не найдено
function getCookie(name) {
    let matches = document.cookie.match(new RegExp(
        "(?:^|; )" + name.replace(/([\.$?*|{}\(\)\[\]\\\/\+^])/g, '\\$1') + "=([^;]*)"
    ));
    return matches ? decodeURIComponent(matches[1]) : undefined;
}

function setCookie(name, value, options = {}) {
    // setCookie('user', 'John', {secure: true, 'max-age': 3600});

    options = {
        path: '/',
        // при необходимости добавьте другие значения по умолчанию
        ...options
    };

    if (options.expires instanceof Date) {
        options.expires = options.expires.toUTCString();
    }

    let updatedCookie = encodeURIComponent(name) + "=" + encodeURIComponent(value);

    for (let optionKey in options) {
        updatedCookie += "; " + optionKey;
        let optionValue = options[optionKey];
        if (optionValue !== true) {
            updatedCookie += "=" + optionValue;
        }
    }

    document.cookie = updatedCookie;
}

