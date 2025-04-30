import asyncio
import logging

from fastapi import APIRouter, Response, WebSocket, WebSocketDisconnect

from cme_vision_api.camera.camera import CameraHandler
from cme_vision_api.config import FLASK_SERVER_HOST, FLASK_SERVER_PORT
from cme_vision_api.services.image_service import ImageSender
from cme_vision_api.services.websocket import WebSocketManager

# Setup logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

router = APIRouter()

# Initialize services
camera_handler = CameraHandler()
websocket_manager = WebSocketManager()
image_sender = ImageSender(
    flask_server_url=f'http://{FLASK_SERVER_HOST}:{FLASK_SERVER_PORT}/classification/classify'
)
# Add a initialization for the objecttrackdetect engine


@router.websocket('/ws/camera/')
async def stream_camera(websocket: WebSocket):
    await websocket_manager.connect(websocket)
    try:
        while True:
            frame = camera_handler.get_frame()

            # Capture frames for frame selection algorithm
            # Frame filtering server-side. May be superseded
            # by client-side frame filtering.
            # Only output the meaningful frame
            # Every frame sent to the machine advances its
            # internal states.
            # if method returns valid_flag for image taken,
            # return the image for a future backend method

            # Do not modify following code for the frontend!
            await websocket_manager.send_frame(frame)
            await asyncio.sleep(0.1)  # Adjust frame rate as needed
    except WebSocketDisconnect:
        websocket_manager.disconnect(websocket)
    except Exception as e:
        logger.error(f"Error in stream_camera: {e}")


@router.get('/capture-and-send')
async def capture_and_send():
    try:
        frame = camera_handler.get_frame()
        flask_response, status_code = await image_sender.send_image(frame)
        return Response(content=flask_response, status_code=status_code)
    except Exception as e:
        logger.error(f"Error in capture-and-send: {e}")
        return Response(content=str(e), media_type="text/plain", status_code=500)
