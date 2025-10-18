#!/bin/bash
# Script to push MicroL4 to GitHub

echo "================================"
echo "MicroL4 GitHub Push Instructions"
echo "================================"
echo ""
echo "This script will help you push the MicroL4 repository to GitHub."
echo ""

# Check if we're in a git repository
if [ ! -d .git ]; then
    echo "ERROR: Not in a git repository. Please run from MicroL4 root directory."
    exit 1
fi

echo "Step 1: Add GitHub remote"
echo "-------------------------"
echo "Run the following command (replace with your actual repo URL):"
echo ""
echo "  git remote add origin https://github.com/AyhamJo7/MicroL4.git"
echo ""
read -p "Press Enter after adding remote..."

echo ""
echo "Step 2: Verify remote"
echo "--------------------"
git remote -v

echo ""
echo "Step 3: Push to main branch"
echo "---------------------------"
echo "Run:"
echo ""
echo "  git push -u origin main"
echo ""
read -p "Press Enter to execute push..."
git push -u origin main

echo ""
echo "Step 4: Create and push release tag"
echo "-----------------------------------"
echo "Run:"
echo ""
echo "  git tag -a v0.1.0 -m 'Initial release: minimal L4 microkernel'"
echo "  git push origin v0.1.0"
echo ""
read -p "Press Enter to create tag..."
git tag -a v0.1.0 -m "Initial release: minimal L4 microkernel"
git push origin v0.1.0

echo ""
echo "================================"
echo "✓ Repository pushed successfully!"
echo "================================"
echo ""
echo "Next steps:"
echo "1. Visit https://github.com/AyhamJo7/MicroL4"
echo "2. Verify CI/CD pipeline runs successfully"
echo "3. Check that README renders correctly"
echo "4. Create a release from tag v0.1.0"
echo ""
