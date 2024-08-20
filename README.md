# AVL Tree Management System

## Overview

This code implements a specialized data structure for managing products based on their time and quality attributes. The data structure maintains two AVL trees:

**🕒 Time Tree:** An AVL tree that organizes products by their time attribute.

**🔧 Quality Tree:** An AVL tree that organizes products by their quality attribute.

The data structure supports operations such as adding and removing products, querying products by rank, and checking for the existence of special qualities.

## Key Features

- **Add Product:** Insert a product with a given time and quality.

- **Remove Product:** Delete a product by time.

- **Remove by Quality:** Delete all products with a specified quality.

- **Query by Rank:** Retrieve the i-th ranked product based on quality.

- **Query by Time Range:** Retrieve the i-th ranked product within a specified time range (time1 to time2).

- **Check Existence:** Determine if a product with a special quality exists.
