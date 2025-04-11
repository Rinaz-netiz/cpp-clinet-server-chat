import asyncio

from websockets.asyncio.server import serve
from websockets.exceptions import ConnectionClosedOK
import json

HOST = 'localhost'
TCP_PORT = 8080
WS_PORT = 8769

async def handler(websocket):
    try:
        reader, writer = await asyncio.open_connection(HOST, TCP_PORT)
    except Exception as e:
        print("Some exception" + e)
        return

    # s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    # s.connect((HOST, TCP_PORT))

    try:
        while True:
                msg = await websocket.recv()
                print(msg)
                writer.write(msg.encode("utf-8"))
                await writer.drain()
                # s.sendall(bytes(msg, "utf-8"))

                response = await reader.read(1024)
                print(f"[TCP] Ответ: {response.decode()}")

                content = {"name": "Karlson", "msg": "crap chat"}
                await websocket.send(json.dumps(content))
    except ConnectionClosedOK:
        print("Клиент отключился")
    finally:
        writer.close()
        await writer.wait_closed()



async def main():
    async with serve(handler, HOST, WS_PORT) as server:
        await server.serve_forever()


if __name__ == "__main__":
    asyncio.run(main())