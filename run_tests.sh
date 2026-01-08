GREEN='\033[0;32m'
CYAN='\033[0;36m'
RED='\033[0;31m'
NC='\033[0m'

echo -e "${CYAN}--- Building Simulator ---${NC}"

g++ main.cpp src/cache/cache.cpp -I include -I src/cache -I include/vm -I src/vm -std=c++11 -o final_sim

if [ $? -ne 0 ]; then
    echo -e "${RED}Compilation failed.${NC}"
    exit 1
fi

echo -e "${CYAN}--- Running Integration Tests ---${NC}"

if [ -f "tests/full_simulation.txt" ]; then
    ./final_sim < tests/full_simulation.txt
else
    echo -e "${RED}Error: tests/full_simulation.txt not found!${NC}"
    exit 1
fi

echo -e "\n-----------------------------------"
echo -e "${GREEN}Test Sequence Complete.${NC}"