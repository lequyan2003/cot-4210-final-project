from fastapi import FastAPI, HTTPException
from fastapi.middleware.cors import CORSMiddleware
from fastapi.responses import FileResponse
from fastapi.staticfiles import StaticFiles
from pydantic import BaseModel
import subprocess
import os

app = FastAPI()

# === CORS settings ===
app.add_middleware(
    CORSMiddleware,
    allow_origins=["http://localhost:3000"],  # Update for production if needed
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

# === Project structure paths ===
BASE_DIR = os.path.dirname(os.path.abspath(__file__))
ROOT_DIR = os.path.join(BASE_DIR, "..")
BUILD_BIN = os.path.join(ROOT_DIR, "build", "main")
VISUAL_DIR = os.path.join(ROOT_DIR, "visualize")
OUTPUT_DIR = os.path.join(ROOT_DIR, "output")

os.makedirs(VISUAL_DIR, exist_ok=True)
os.makedirs(OUTPUT_DIR, exist_ok=True)

# === Request models ===
class GenerateRequest(BaseModel):
    regex: str
    minimized: bool = False

class SimulateRequest(BaseModel):
    regex: str
    input: str

# === POST /generate ===
@app.post("/generate")
async def generate(req: GenerateRequest):
    try:
        subprocess.run(
            [BUILD_BIN, "--visualize", req.regex],
            check=True, capture_output=True, text=True, cwd=ROOT_DIR
        )
        if req.minimized:
            subprocess.run(
                [BUILD_BIN, "--visualize-min", req.regex],
                check=True, capture_output=True, text=True, cwd=ROOT_DIR
            )
        return {"status": "ok", "message": "Visuals generated"}
    except subprocess.CalledProcessError as e:
        raise HTTPException(status_code=500, detail=e.stderr)

# === POST /simulate ===
@app.post("/simulate")
async def simulate(req: SimulateRequest):
    try:
        result = subprocess.run(
            [BUILD_BIN, "--simulate", req.regex, req.input, "--trace"],
            check=True, capture_output=True, text=True, cwd=ROOT_DIR
        )
        return {"status": "ok", "trace": result.stdout}
    except subprocess.CalledProcessError as e:
        raise HTTPException(status_code=500, detail=e.stderr)

# === GET /visuals/nfa|dfa|min_dfa ===
@app.get("/visuals/{type}")
async def get_visual(type: str):
    file_map = {
        "nfa": "nfa_visual.png",
        "dfa": "dfa_visual.png",
        "min_dfa": "min_dfa_visual.png",
    }
    filename = file_map.get(type)
    path = os.path.join(VISUAL_DIR, filename) if filename else None
    if not path or not os.path.exists(path):
        raise HTTPException(status_code=404, detail=f"{type}.png not found")
    return FileResponse(path, media_type="image/png")

# === GET /json/nfa|dfa|min_dfa ===
@app.get("/json/{type}")
async def get_json(type: str):
    file_map = {
        "nfa": "nfa.json",
        "dfa": "dfa.json",
        "min_dfa": "min_dfa.json",
    }
    filename = file_map.get(type)
    path = os.path.join(OUTPUT_DIR, filename) if filename else None
    if not path or not os.path.exists(path):
        raise HTTPException(status_code=404, detail=f"{type}.json not found")
    return FileResponse(path, media_type="application/json")

# === Optional static mounts ===
app.mount("/output", StaticFiles(directory=OUTPUT_DIR), name="output")
app.mount("/visualize", StaticFiles(directory=VISUAL_DIR), name="visualize")
