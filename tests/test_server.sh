#!/bin/bash

# Simple test script for a web server
# Tests basic functionality with different HTTP methods and URLs

# Configuration
SERVER="localhost"
PORT="8080"
TIMEOUT=5  # Timeout in seconds for requests

# Colors for better readability
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[0;33m'
NC='\033[0m' # No Color

# Function to print test results
test_result() {
    if [ $1 -eq 0 ]; then
        echo -e "${GREEN}✓ PASS${NC}: $2"
    else
        echo -e "${RED}✗ FAIL${NC}: $2 (Error code: $1)"
    fi
}

# Function to make a request and validate response
make_request() {
    local method=$1
    local path=$2
    local expected_status=$3
    local data=$4
    
    echo -e "\n${YELLOW}=== Testing $method $path ===${NC}"
    
    local cmd="curl -s -o response.txt -w '%{http_code}' -X $method http://$SERVER:$PORT$path"
    
    # Add data if provided
    if [ ! -z "$data" ]; then
        cmd="$cmd -d '$data' -H 'Content-Type: application/x-www-form-urlencoded'"
    fi
    
    # Add timeout
    cmd="$cmd --connect-timeout $TIMEOUT"
    
    # Execute command and get status code
    local status_code=$(eval $cmd)
    local exit_code=$?
    
    # Check for connection errors
    if [ $exit_code -ne 0 ]; then
        echo -e "${RED}Connection error. Is the server running on $SERVER:$PORT?${NC}"
        return $exit_code
    fi
    
    # Check status code
    if [ "$status_code" == "$expected_status" ]; then
        test_result 0 "$method $path returned $status_code as expected"
    else
        test_result 1 "$method $path returned $status_code, expected $expected_status"
    fi
    
    # Display response body
    echo "Response body:"
    cat response.txt
    echo ""
    
    return 0
}

# Main test sequence
echo "=== Web Server Test Script ==="
echo "Testing server at $SERVER:$PORT"

# Test basic GET request
make_request "GET" "/" 200

# Test 404 case
make_request "GET" "/nonexistent" 404

# Test different HTTP methods
make_request "HEAD" "/" 200
make_request "POST" "/" 200 "data=test&param=value"
make_request "PUT" "/" 200 "content=updated"
make_request "DELETE" "/" 200

# Test different content types
make_request "GET" "/index.html" 200
make_request "GET" "/style.css" 200
make_request "GET" "/script.js" 200
make_request "GET" "/image.jpg" 200

# Test larger payload
make_request "POST" "/submit" 200 "$(cat <<EOF
payload=This is a larger payload that spans multiple lines
and should be properly handled by the server.
It tests the ability to handle larger requests.
EOF
)"

# Clean up
rm -f response.txt

echo -e "\n${YELLOW}=== All tests completed ===${NC}"
