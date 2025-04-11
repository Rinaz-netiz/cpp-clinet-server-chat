#!/usr/bin/env python

import asyncio
import socket
from websockets.asyncio.server import serve
from websockets.exceptions import ConnectionClosedOK
import json

HOST = 'localhost'
TCP_PORT = 8080
WS_PORT = 8769

async def handler(websocket):
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((HOST, TCP_PORT))

    while True:
        try:
            await websocket.send("Hi")
            msg = await websocket.recv()
            print(msg)
        except ConnectionClosedOK:
            break



async def main():
    async with serve(handler, HOST, WS_PORT) as server:
        await server.serve_forever()


if __name__ == "__main__":
    asyncio.run(main())