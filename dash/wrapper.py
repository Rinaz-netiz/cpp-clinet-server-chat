#!/usr/bin/env python

import asyncio
import socket
from websockets.asyncio.server import serve
from websockets.exceptions import ConnectionClosedOK

HOST = 'localhost'
TCP_PORT = 9999
WS_PORT = 8764

async def handler(websocket):
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((HOST, TCP_PORT))
    print("send request")

    while True:
        try:
            message = await websocket.recv()
            s.sendall(bytes(message, 'utf-8'))
        except ConnectionClosedOK:
            break
        print(message)


async def main():
    async with serve(handler, HOST, WS_PORT) as server:
        await server.serve_forever()


if __name__ == "__main__":
    asyncio.run(main())