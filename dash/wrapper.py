import asyncio
import socket

from websockets.asyncio.server import serve
from websockets.exceptions import ConnectionClosedOK
import json

HOST = 'localhost'
TCP_PORT = 8080
WS_PORT = 8700

async def handler(websocket):
    try:
        reader, writer = await asyncio.open_connection(HOST, TCP_PORT)
    except Exception as e:
        print("Can't create connection to server:" + str(e))
        return

    try:
        while True:
                msg = await websocket.recv()
                writer.write(msg.encode("utf-8"))
                await writer.drain()

                response = await reader.read(1024)

                await websocket.send(json.dumps(json.loads(response.decode())))

    except ConnectionClosedOK:
        print("Клиент отключился")

        writer.close()
        await writer.wait_closed()



async def main():
    async with serve(handler, HOST, WS_PORT) as server:
        await server.serve_forever()


if __name__ == "__main__":
    asyncio.run(main())