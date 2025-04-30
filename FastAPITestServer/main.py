# main.py -
# testando templates e páginas estáticas para projeto com esp32
import asyncio
import shutil
import socket
from contextlib import asynccontextmanager
from datetime import datetime
from pathlib import Path

from fastapi import FastAPI, File, Request, UploadFile
from fastapi.responses import HTMLResponse, JSONResponse
from fastapi.staticfiles import StaticFiles
from fastapi.templating import Jinja2Templates
from sse_starlette.sse import EventSourceResponse

BASE = Path(__file__).parent
PH = BASE / 'static' / 'placeholders'
CUR = BASE / 'static' / 'current'
# An asyncio.Event used to signal “new upload”
upload_event = asyncio.Event()


def get_local_ip() -> str:
    """Return the primary LAN IP address of this machine."""
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    try:
        # connect to an arbitrary public IP, no packets are actually sent
        s.connect(('8.8.8.8', 80))
        return s.getsockname()[0]
    except Exception:
        return '127.0.0.1'
    finally:
        s.close()


# 1) Define a lifespan function
@asynccontextmanager
async def lifespan(app: FastAPI):
    # ─── Startup block ────────────────────────────────
    CUR.mkdir(parents=True, exist_ok=True)
    for img in ('qvga.jpg', 'mask.jpg', 'uxga.jpg'):
        src = PH / img
        dst = CUR / img
        shutil.copy2(src, dst)
    # Hand control back to FastAPI
    ip = get_local_ip()
    print(f'📡  Server listening at http://{ip}:8000')
    app.state.last_upload = None  # initialize
    yield
    # ─── Shutdown block (optional) ────────────────────
    # e.g. cleanup tasks, if any


# 2) Create your FastAPI app with that lifespan
app = FastAPI(lifespan=lifespan)

# Set up template rendering
templates = Jinja2Templates(directory='templates')


@app.middleware('http')
async def no_cache_current_images(request, call_next):
    response = await call_next(request)
    # if request.url.path.startswith('/static/current/'):
    if (request.url.path.startswith("/static/current/") and
        request.url.path.endswith((".jpg", ".jpeg"))):
        # force the browser to ALWAYS re-fetch
        responsestr = 'no-store, no-cache, must-revalidate, max-age=0'
        response.headers['Cache-Control'] = responsestr
    return response


# Mount static files (for CSS, JS, images, etc.)
app.mount('/static', StaticFiles(directory='static'), name='static')
# app.mount(
#     '/static',
#     StaticFiles(directory='static', html=True, check_dir=True),
#     name='static',
# )


# A simple endpoint to render the homepage
@app.get('/', response_class=HTMLResponse)
async def read_root(request: Request):
    # Render an HTML file from the "templates" folder
    # return templates.TemplateResponse('index.html', {'request': request})
    ts = (
        app.state.last_upload.strftime("%Y-%m-%d %H:%M:%S")
        if app.state.last_upload else None
    )
    return templates.TemplateResponse(
      'index.html',
      {'request': request, 'timestamp': ts}
    )


# 3) Your existing upload endpoint (unchanged)
@app.post('/upload')
async def upload_images(
    grayscale: UploadFile = File(...),
    mask: UploadFile = File(...),
    uxga: UploadFile = File(...),
):
    for field, fname in (
        (grayscale, 'qvga.jpg'),
        (mask, 'mask.jpg'),
        (uxga, 'uxga.jpg'),
    ):
        out_path = CUR / fname
        content = await field.read()  # bytes
        with open(out_path, 'wb') as out:
            out.write(content)
    app.state.last_upload = datetime.now()
    upload_event.set()
    # upload_event.clear()
    return JSONResponse({'status': 'ok'})


# Endpoint for a simple status or control API
@app.get('/status')
async def status():
    return {'message': 'Server is running'}


# 4) Your SSE endpoint (unchanged)
@app.get('/events')
async def events():
    async def gen():
        while True:
            # wait until someone uploads
            await upload_event.wait()
            # notify the browser
            yield 'data: refresh\n\n'
            # now reset the flag so we can catch the NEXT upload
            upload_event.clear()

    # return EventSourceResponse(gen())
    return EventSourceResponse(
        gen(),
        headers={
            'Cache-Control': 'no-cache',  # always revalidate
            'X-Accel-Buffering': 'no',  # if you ever sit behind nginx
        },
    )
