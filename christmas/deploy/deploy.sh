#!/bin/bash

# 🎄 Christmas CGI Web App - Quick Deploy Script
# Usage: ./deploy.sh [host] [username] [remote_path]

set -e

echo "🎄 Christmas CGI Web App - Deployment Script"
echo "============================================="

# Default values
HOST=${1:-"yourserver.com"}
USER=${2:-"yourusername"}
REMOTE_PATH=${3:-"/var/www/cgi-bin"}

echo "Deploying to: $USER@$HOST:$REMOTE_PATH"
echo ""

# Check if main CGI file exists
if [ ! -f "main.cgi" ]; then
    echo "❌ Error: main.cgi not found. Run 'make main.cgi' first."
    exit 1
fi

# Create remote directory
echo "📁 Creating remote directory..."
ssh "$USER@$HOST" "mkdir -p $REMOTE_PATH"

# Upload files
echo "📤 Uploading CGI files..."
scp main.cgi "$USER@$HOST:$REMOTE_PATH/"

# Set permissions
echo "🔧 Setting permissions..."
ssh "$USER@$HOST" "chmod +x $REMOTE_PATH/main.cgi"

# Test deployment
echo "🧪 Testing deployment..."
if ssh "$USER@$HOST" "test -x $REMOTE_PATH/main.cgi"; then
    echo "✅ Deployment successful!"
    echo ""
    echo "🌐 Your Christmas Mini Market is live at:"
    echo "   Main Marketplace: https://$HOST/cgi-bin/main.cgi"
    echo "   Tree Generator:   https://$HOST/cgi-bin/main.cgi?action=tree"
    echo "   Card Creator:     https://$HOST/cgi-bin/main.cgi?action=card"
    echo "   Secret Santa:     https://$HOST/cgi-bin/main.cgi?action=santa"
    echo "   Countdown:        https://$HOST/cgi-bin/main.cgi?action=countdown"
    echo "   About:            https://$HOST/cgi-bin/main.cgi?action=about"
else
    echo "❌ Deployment failed!"
    exit 1
fi

echo ""
echo "🎅 Merry Christmas! Your web app is ready to spread holiday cheer! 🎄"