@echo off
python ./api/tools/genEmmyLua.py ./api
python ./api/tools/genCompletions.py --src ./src --out ./api