#!/bin/bash
set -e

# Configuration
REMOTE_USER="louis"
REMOTE_HOST="192.168.1.44"
REMOTE_DIR="~/rpi-rgb-led-matrix" # Verify this path on your Pi

echo "🚀 Deploying to $REMOTE_HOST..."

git add . && git commit --amend --no-edit
# 1. Push changes
echo "pushing changes..."
git push origin master --force

# 2. Run on Pi
echo "running on remote..."
ssh -t $REMOTE_USER@$REMOTE_HOST <<EOF
    cd $REMOTE_DIR || exit 1
    echo "📍 In directory: \$(pwd)"
    
    echo "⬇️ Pulling changes..."
    git fetch origin master
    git reset --hard origin/master
    
    echo "🔨 Building NIF..."
    make -C bindings/erlang
    
    echo "▶️ Running module..."
    # Access needs sudo for GPIO
    sudo elixir -r bindings/erlang/module.ex -e 'Run.run()'
EOF
