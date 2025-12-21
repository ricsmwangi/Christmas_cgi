#!/bin/bash

# Test script for Santa's Christmas CGI Web Generator
# Run with: ./test_cgi.sh

echo "🎄 Testing Santa's Christmas CGI Web Generator"
echo "=============================================="

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Check if CGI programs exist
programs=("tree.cgi" "card.cgi" "santa.cgi" "countdown.cgi")

for prog in "${programs[@]}"; do
    if [ -f "$prog" ]; then
        echo -e "${GREEN}✓${NC} $prog exists"
    else
        echo -e "${RED}✗${NC} $prog missing - run 'make' first"
        exit 1
    fi
done

echo ""
echo "🧪 Testing CGI programs..."

# Test tree.cgi with query parameters
echo "Testing tree.cgi..."
QUERY_STRING="height=5&ornaments=*!&color=red" ./tree.cgi > /dev/null 2>&1
if [ $? -eq 0 ]; then
    echo -e "${GREEN}✓${NC} tree.cgi basic test passed"
else
    echo -e "${RED}✗${NC} tree.cgi test failed"
fi

# Test card.cgi with POST data
echo "Testing card.cgi..."
echo "recipient=Test&message=Hello&submit=1" | ./card.cgi > /dev/null 2>&1
if [ $? -eq 0 ]; then
    echo -e "${GREEN}✓${NC} card.cgi basic test passed"
else
    echo -e "${RED}✗${NC} card.cgi test failed"
fi

# Test countdown.cgi
echo "Testing countdown.cgi..."
./countdown.cgi > /dev/null 2>&1
if [ $? -eq 0 ]; then
    echo -e "${GREEN}✓${NC} countdown.cgi basic test passed"
else
    echo -e "${RED}✗${NC} countdown.cgi test failed"
fi

# Test santa.cgi
echo "Testing santa.cgi..."
echo "participants=Alice,Bob,Charlie&submit=1" | ./santa.cgi > /dev/null 2>&1
if [ $? -eq 0 ]; then
    echo -e "${GREEN}✓${NC} santa.cgi basic test passed"
else
    echo -e "${RED}✗${NC} santa.cgi test failed"
fi

echo ""
echo "🌐 To test in browser:"
echo "1. Run: make test-server"
echo "2. Open: http://localhost:8000/cgi-bin/tree.cgi?height=8"
echo "3. Try other CGI programs with different parameters"

echo ""
echo "🎅 Merry Christmas and Happy Coding!"