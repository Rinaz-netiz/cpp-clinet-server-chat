window.addEventListener("DOMContentLoaded", () => {
    // Open the WebSocket connection and register event handlers.
    const websocket = new WebSocket("ws://localhost:8001/");
    sendMoves(board, websocket);
});
//
// function sendMoves(board, websocket) {
//     // When clicking a column, send a "play" event for a move in that column.
//     board.addEventListener("click", ({ target }) => {
//         const column = target.dataset.column;
//         // Ignore clicks outside a column.
//         if (column === undefined) {
//             return;
//         }
//         const event = {
//             type: "play",
//             column: parseInt(column, 10),
//         };
//         websocket.send(JSON.stringify(event));
//     });
// }



function post(path, params, method='post') {
    // The rest of this code assumes you are not using a library.
    // It can be made less verbose if you use one.
    const form = document.createElement('form');
    form.method = method;
    form.action = path;
    for (const key in params) {
        if (params.hasOwnProperty(key)) {
            const hiddenField = document.createElement('input');
            hiddenField.type = 'hidden';
            hiddenField.name = key;
            hiddenField.value = params[key];
            form.appendChild(hiddenField);
        }
    }
    document.body.appendChild(form);
    form.submit();
}