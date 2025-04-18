import asyncio

from websockets.asyncio.server import serve
from websockets.exceptions import ConnectionClosedOK, ConnectionClosed
import json

HOST = 'localhost'
TCP_PORT = 8080
WS_PORT = 8733


CLIENTS = set()

async def broadcast(message, sender):
    for websocket in CLIENTS:
        if websocket != sender:
            await send(websocket, message)

async def send(websocket, message):
    try:
        await websocket.send(message)
    except ConnectionClosed:
        print("ConnectionClosed")

async def handler(websocket):
    try:
        reader, writer = await asyncio.open_connection(HOST, TCP_PORT)
    except Exception as e:
        print("Can't create connection to server:" + str(e))
        return

    CLIENTS.add(websocket)

    try:
        while True:
                msg = await websocket.recv()

                await broadcast(msg, websocket)

                writer.write(msg.encode("utf-8"))
                await writer.drain()

                response = await reader.read(1024)

                data_from_server = json.dumps(json.loads(response.decode()))


    except ConnectionClosedOK:
        print("Клиент отключился")

        # writer.close()
        # await writer.wait_closed()
    finally:
        CLIENTS.remove(websocket)



async def main():
    async with serve(handler, HOST, WS_PORT) as server:
        await server.serve_forever()


if __name__ == "__main__":
    asyncio.run(main())