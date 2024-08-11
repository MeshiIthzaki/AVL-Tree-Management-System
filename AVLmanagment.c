#include <stdio.h>
#include <stdlib.h>

#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))
#define EXTRA 10

/* Product struct */
typedef struct Product
{
    int time;
    int quality;
    struct Product* parent;
    struct Product* left;
    struct Product* right;
    struct Product* twin;           /* points to twin product in quality / time tree */
    struct Product* timeSubtree;    /* for quality tree, a pointer to same quality dif times subtree */
    struct Product* minQualityP;    /* points to the min quality product in subtree */
    int height;                     /* height of product in AVL tree */
    int subtreeSize;                /* products subtree size */
    int minQuality;                 /* value of minimum quality in subtree */
} Product;

/* Data Structre struct */
typedef struct DataStructure {
    Product* timeRoot;         /* pointer to time AVL tree */
    Product* qualityRoot;      /* pointer to quality AVL tree */
    int special;               /* keeps special quality */
    int specialExists;         /* 1 if special quality exists, 0 otherwise*/
} DataStructure;

/*--------------- DECLARATIONS ---------------*/

/* Data Structre functions */
DataStructure Init(int s);
void AddProduct(DataStructure* ds, int time, int quality);
void RemoveProduct(DataStructure* ds, int time);
void RemoveQuality(DataStructure* ds, int quality);
int GetIthRankProduct(DataStructure ds, int i);
int GetIthRankProductBetween(DataStructure ds, int time1, int time2, int i);
int Exists(DataStructure ds);
/* Time tree functions */
Product* creatNewProduct(int newTime, int newQuality);
Product* searchTime(Product* root, int time);
Product* insertTime(Product* root, Product* x);
Product* removeProductFromTime(Product* root, int time);
Product* findIthTime(Product* root, int i);
/* Quality tree functions */
Product* createQualityNode(int newQuality);
Product* searchQuality(Product* root, int quality);
Product* insertQuality(Product* root, Product* x);
Product* removeProductFromQuality(Product* root, int time, int quality);
Product* findIthQuality(Product* root, int i);
/* AVL general functions */
void swapProduct(Product* a, Product* b);
int height(Product* x);
void updateHeight(Product* x);
Product* minProduct(Product* root);
Product* maxProduct(Product* root);
Product* minOfTwoProducts(Product* x, Product* y);
Product* minQualityPointer(Product* x, Product* y);
void updateMinQuality(Product* x);
Product* rightRotate(Product* x);
Product* leftRotate(Product* x);
Product* balance(Product* x);
int timeSubtreeSize(Product* qualityRoot);
Product* findTimeOrSuccessor(Product* root, int time);
Product* findTimeOrPredecessor(Product* root, int time);
Product* minProductLeft(Product* root, Product* min, int time1, int time2);
Product* minProductRight(Product* root, Product* min, int time1, int time2);
Product* findMinQualityBetween(Product* root, int left, int right);
int isInRange(Product* x, int time1, int time2);
int countProducts(Product* root, int time1, int time2);
Product* updateToNewQuality(Product* root, int time, int newQuality);

/*--------------- DATA STRACTURE ---------------*/

/* FUNCTION 1 - initiallize data structure and return it */
DataStructure Init(int s)
{
    DataStructure newDS;
    newDS.timeRoot = NULL;
    newDS.qualityRoot = NULL;
    newDS.special = s;
    newDS.specialExists = 0;
    return newDS;
}

/* FUNCTION 2 - adds a product to time tree and quality tree (O(logn)) */
void AddProduct(DataStructure* ds, int time, int quality)
{
    Product *timeProduct, *newTimeRoot;
    Product *qualityProduct, *newQualityRoot;

    /* create a new product and insert to time tree (O(logn)) */
    timeProduct = creatNewProduct(time, quality);
    newTimeRoot = insertTime(ds->timeRoot, timeProduct);
    ds->timeRoot = newTimeRoot;

    /* create a new product and insert to quality tree (O(logn)) */
    qualityProduct = creatNewProduct(time, quality);
    newQualityRoot = insertQuality(ds->qualityRoot, qualityProduct);
    ds->qualityRoot = newQualityRoot;

    /* update twin pointers */
    timeProduct->twin = qualityProduct;
    qualityProduct->twin = timeProduct;

    /* check if special quality */
    if (quality == ds->special) ds->specialExists = 1;
}

/* FUNCTION 3 - remove a product by time from both trees (O(logn)) */
void RemoveProduct(DataStructure* ds, int time)
{
    Product *productToDelete, *newTimeRoot, *newQualityRoot, *qualitySearch;
    int quality;
    
    /* find product to remove's quality (O(logn)) */
    productToDelete = searchTime(ds->timeRoot, time);
    if (productToDelete == NULL || productToDelete->time != time) return;   /* product not found */
    quality = productToDelete->quality;                                 /* get products quality */

    /* remove from Time tree (O(logn)) */
    newTimeRoot = removeProductFromTime(ds->timeRoot, time);
    ds->timeRoot = newTimeRoot;

    /* remove from Quality tree (O(logn)) */
    newQualityRoot = removeProductFromQuality(ds->qualityRoot, time, quality);
    ds->qualityRoot = newQualityRoot;

    /* check if special quality exists (O(logn)) */
    if (quality == ds->special)
    {
        qualitySearch = searchQuality(ds->qualityRoot, quality);
        if (qualitySearch == NULL || qualitySearch->quality != quality) ds->specialExists = 0;
    }
}

/* FUNCTION 4 - removes all products with specific quality O((klogn)) */
void RemoveQuality(DataStructure* ds, int quality)
{
    Product *qualityNode, *newTimeRoot, *newQualityRoot;
    int qualityExists, currentTime;

    /* check if special quality */
    if (ds->special == quality) ds->specialExists = 0;

    /* search for quality node (O(logn)) */
    qualityNode = searchQuality(ds->qualityRoot, quality);
    if (qualityNode == NULL || qualityNode->quality != quality) return; /* product not found */
    else qualityExists = 1;

    /* delete all products with quality from both trees (O(klogn)) */
    while (qualityExists) /* k times */
    {
        currentTime = qualityNode->timeSubtree->time; /* find subtimeRoot time */

        /* remove from Time tree (O(logn)) */
        newTimeRoot = removeProductFromTime(ds->timeRoot, currentTime);
        ds->timeRoot = newTimeRoot;

        /* remove from Quality tree (O(logn)) */
        newQualityRoot = removeProductFromQuality(ds->qualityRoot, currentTime, quality);
        ds->qualityRoot = newQualityRoot;

        /* check if quality still exits (O(logn)) */
        qualityNode = searchQuality(ds->qualityRoot, quality);
        if (qualityNode == NULL || qualityNode->quality != quality) qualityExists = 0;
    }
}

/* FUNCTION 5 - returns the i-th rank product's time */
int GetIthRankProduct(DataStructure ds, int i)
{
    Product *ithProduct;
    /* empty tree */
    if (ds.qualityRoot == NULL) return -1;

    /* find the ith rank product (O(logn)) */
    ithProduct = findIthQuality(ds.qualityRoot, i);

    if (ithProduct == NULL) return -1;      /* if doesnt exist */
    else return ithProduct->time;
}

/* FUNCTION 6 - returns the i-th rank product's time between t1 and t2*/
int GetIthRankProductBetween(DataStructure ds, int time1, int time2, int i)
{
    Product *minProduct;
    int left, right, ithTime, counter, maxQuality, j;
    int *minTime, *minQuality;

    /* update bounds */
    left = min(time1, time2);
    right = max(time1, time2);
    left = findTimeOrSuccessor(ds.timeRoot, left)->time;
    right = findTimeOrPredecessor(ds.timeRoot, right)->time;

    counter = countProducts(ds.timeRoot, left, right);  /* count how many products are between time1 and time2 */

    /* input check */
    if (ds.timeRoot == NULL) return -1;                                    /* empty ds */
    if (i < 1 || i > ds.timeRoot->subtreeSize || i > counter) return -1;   /* i range check */

    /* array for time of minimum products */
    minTime = (int*)malloc(i * sizeof(int));
    if (minTime == NULL) return -1;

    /* array for quality of minimum products */
    minQuality = (int*)malloc(i * sizeof(int));
    if (minQuality == NULL) return -1;

    for (j = 0; j < i; j++)        /* i times */
    {
        /* find the minimum quality node between t1 and t2 (O(logn)) */
        minProduct = findMinQualityBetween(ds.timeRoot, left, right);

        /* find maximum quality that exists and add EXTRA */
        maxQuality = maxProduct(ds.qualityRoot)->quality + EXTRA;

        /* add current minimum to arrays */
        minTime[j] = minProduct->time;
        minQuality[j] = minProduct->quality;

        /* update minimum quality to max */
        ds.timeRoot = updateToNewQuality(ds.timeRoot, minProduct->time, maxQuality);

        /* update bounds */
        if (j != i-1)
        {
            left = findTimeOrSuccessor(ds.timeRoot, left)->time;
            right = findTimeOrPredecessor(ds.timeRoot, right)->time;
        }
    }

    /* keep return value */
    ithTime = minTime[i-1];

    /* update products quality back to original */
    for (j = 0; j < i; j++)
    {
        ds.timeRoot = updateToNewQuality(ds.timeRoot, minTime[j], minQuality[j]);
    }
    free(minTime);
    free(minQuality);
    return ithTime;
}

/* FUNCTION 7 - returns 1 if a product with special quality exists, 0 otherwise (O(1)) */
int Exists(DataStructure ds)
{
    return ds.specialExists;
}

/*--------------- HELPER FUNCTIONS ----------------*/

/* creates a new Product and returns it (O(1))*/
Product* creatNewProduct(int newTime, int newQuality)
{
    /* allocate memory for new product */
    Product* newProduct = (Product*)malloc(sizeof(Product));
    if (newProduct == NULL) return NULL;
    newProduct->quality = newQuality;
    newProduct->time = newTime;
    newProduct->left = NULL;
    newProduct->right = NULL;
    newProduct->parent = NULL;
    newProduct->timeSubtree = NULL;
    newProduct->minQualityP = newProduct;
    newProduct->height = 0;
    newProduct->subtreeSize = 1;
    newProduct->minQuality = newQuality;
    return newProduct;
}

/* creates a new quality node and returns it (O(1))*/
Product* createQualityNode(int newQuality)
{
    /* allocate memory for new quality node */
    Product *newQualityNode = (Product*)malloc(sizeof(Product));
    if (newQualityNode == NULL) return NULL;
    newQualityNode->quality = newQuality;
    newQualityNode->time = -1;                  /* irrelevent for this type of node */
    newQualityNode->left = NULL;
    newQualityNode->right = NULL;
    newQualityNode->parent = NULL;
    newQualityNode->timeSubtree = NULL;
    newQualityNode->minQualityP = NULL;         /* irrelevent for this type of node */
    newQualityNode->height = 0;
    newQualityNode->subtreeSize = 0;
    newQualityNode->minQuality = newQuality;    /* irrelevent for this type of node */
    return newQualityNode;
}

/* swaps between two products (O(1)) */
void swapProduct(Product* a, Product* b)
{
    Product* temp;

    /* update time and quality */
    a->time = b->time;
    a->quality = b->quality;

    /* swap time subtree */
    temp = a->timeSubtree;
    a->timeSubtree = b->timeSubtree;
    b->timeSubtree = temp;
}

/* search a product in time tree and returns a pointer to found product / its successor or predeccessor (O(logn)) */
Product* searchTime(Product* root, int time)
{
    Product *y = NULL;
    Product *z = root;
    while (z != NULL)
    {
        y = z;
        if (time == z->time) return z;
        if (time < z->time) z = z->left;
        else z = z->right;
    }
    return y;
}

/* search a product in quality tree and return a pointer to found node / its successor or predecessor (O(logn)) */
Product* searchQuality(Product* root, int quality)
{
    Product *y = NULL;
    Product *z = root;
    while (z != NULL)
    {
        y = z;
        if (quality == z->quality) return z;
        if (quality < z->quality) z = z->left;
        else z = z->right;
    }
    return y;
}

/* returns a nodes height (O(1)) */
int height(Product* x)
{
    int leftHeight, rightHeight;

    /* if product is NULL, height is -1 */
    if (x == NULL) return -1;

    /* get the height of the subtrees and return the max height + 1*/
    leftHeight = (x->left ? x->left->height : -1);
    rightHeight = (x->right ? x->right->height : -1);
    return (max(leftHeight, rightHeight) + 1);
}

/* gets a product and updates its height (O(1)) */
void updateHeight(Product* x)
{
    x->height = height(x);
}

/* gets two products and returns the smaller product by quality & time (O(1)) */
Product* minOfTwoProducts(Product* x, Product* y)
{
    if (x == NULL) return y;
    if (y == NULL) return x;

    /* compare by quality */
    if (x->quality < y->quality) return x;
    if (x->quality > y->quality) return y;

    /* compare by time */
    if (x->quality == y->quality) return (x->time < y->time ? x : y);
}

/* returns the minimum quality product of two nodes - for delete (O(1)) */
Product* minQualityPointer(Product* x, Product* y)
{
    if (x == NULL || y == NULL) return NULL;

    /* if we deleted the minimum quality */
    if (x->minQualityP == NULL && y->minQualityP) return minOfTwoProducts(x, y->minQualityP);
    if (x->minQualityP && y->minQualityP == NULL) return minOfTwoProducts(x->minQualityP, y);

    /* both pointing to NULL */
    if (y->minQualityP == NULL && x->minQualityP == NULL) return minOfTwoProducts(x, y);

    /* if both still pointing to the minimum */
    else return minOfTwoProducts(x->minQualityP, y->minQualityP);
}

/* updating minimum quality pointer (O(1)) */
void updateMinQuality(Product* x)
{
    Product* temp;
    if (x == NULL) return;

    /* no subtree - x is the minimum */
    if (x->left == NULL && x->right == NULL)
    {
        x->minQualityP = x;
        x->minQuality = x->quality;
    }
    /* if x has both children */
    if (x->right && x->left)
    {
        /* comparing the children, then comparing minimum child with parent */
        temp = minQualityPointer(x->right, x->left);
        x->minQualityP = minQualityPointer(x, temp);
        x->minQuality = x->minQualityP->quality;
    }
    /* if x has only one child */
    else if (x->left || x->right)
    {
        temp = (x->right ? x->right : x->left);
        x->minQualityP = minQualityPointer(x, temp);
        x->minQuality = x->minQualityP->quality;
    }
}

/* right rotation (O(1)) */
Product* rightRotate(Product* x)
{
    /* update left and right pointers */
    Product* y = x->left;
    x->left = y->right;
    y->right = x;

    /* update parents */
    if (x->parent)
    {
        /* if x is a left child */
        if (x->parent->left == x) x->parent->left = y;
        /* if x is a right child */
        if (x->parent->right == x) x->parent->right = y;
    }
    y->parent = x->parent;
    x->parent = y;

    /* update heights */
    updateHeight(x);
    updateHeight(y);

    /* update subtree size */
    if (x->left) x->subtreeSize = x->subtreeSize - y->subtreeSize + x->left->subtreeSize ;
    else x->subtreeSize = x->subtreeSize - y->subtreeSize;

    if (x->right) y->subtreeSize = y->subtreeSize + x->right->subtreeSize + 1;
    else y->subtreeSize++;

    /* update min quality in subtree */
    updateMinQuality(y);
    updateMinQuality(x);

    return y;
}

/* left rotation (O(1)) */
Product* leftRotate(Product* x)
{
    /* update left and right pointers */
    Product* y = x->right;
    x->right = y->left;
    y->left = x;

    /* update parent */
    if (x->parent)
    {
        /* if x is a left child */
        if (x->parent->left == x) x->parent->left = y;
        /* if x is a right child */
        if (x->parent->right == x) x->parent->right = y;
    }
    y->parent = x->parent;
    x->parent = y;

    /* update heights */
    updateHeight(x);
    updateHeight(y);

    /* update subtree size */
    if (x->right) x->subtreeSize = x->subtreeSize - y->subtreeSize + x->right->subtreeSize;
    else x->subtreeSize = x->subtreeSize - y->subtreeSize;

    if (x->left) y->subtreeSize = y->subtreeSize + x->left->subtreeSize + 1;
    else y->subtreeSize++;

    /* update min quality in subtree */
    updateMinQuality(x);
    updateMinQuality(y);

    return y;
}

/* balance the tree and return new root (O(1)) */
Product* balance(Product* x)
{
    Product* y;

    /* if already balanced */
    if (abs(height(x->left) - height(x->right)) <= 1) return x;

    /* if x is left heavy */
    else if (height(x->left) > height(x->right))
    {
        y = x->left;
        /* if x is left right heavy */
        if (height(y->left) < height(y->right)) leftRotate(y);
        return rightRotate(x);
    }
    /* if x is right heavy */
    else
    {
        y = x->right;
        /* if x is right left heavy */
        if (height(y->left) > height(y->right)) rightRotate(y);
        return leftRotate(x);
    }
}

/* insert a product to time tree and return new root (O(logn)) */
Product* insertTime(Product* root, Product* x)
{
    Product* y;

    /* base case */
    if (root == NULL)
    {
        x->height = 0;
        return x;
    }
    /* insert product to the left subtree */
    if (x->time < root->time)
    {
        y = insertTime(root->left, x);
        root->left = y;
        y->parent = root;
        root->height = y->height + 1;   /* update height */
    }
    /* insert product to the right subtree */
    else
    {
        y = insertTime(root->right, x);
        root->right = y;
        y->parent = root;
        root->height = y->height + 1;   /* update height */
    }
    /* increment subtree size */
    root->subtreeSize++;

    /* update minimum quality in subtree */
    updateMinQuality(root);

    /* balance the tree */
    root = balance(root);

    return root;
}

/* insert a product to quality tree and return new root (O(logn)) */
Product* insertQuality(Product* root, Product* x)
{
    Product* y, *newQuality, *newTimeRoot;

    /* base case */
    if (root == NULL)
    {
        newQuality = createQualityNode(x->quality);                     /* create a quality node */
        newTimeRoot = insertTime(newQuality->timeSubtree, x);           /* add product to quality's time subtree */
        newQuality->timeSubtree = newTimeRoot;                          /* update new time subtree root */
        newQuality->subtreeSize += newTimeRoot->subtreeSize;            /* add time subtree to subtree size */
        return newQuality;
    }
    /* insert product to the left subtree */
    if (x->quality < root->quality)
    {
        y = insertQuality(root->left, x);
        root->left = y;
        y->parent = root;
        root->height = y->height + 1;   /* update height */
    }
    /* insert product to the right subtree */
    if (x->quality > root->quality)
    {
        y = insertQuality(root->right, x);
        root->right = y;
        y->parent = root;
        root->height = y->height + 1;   /* update height */
    }
    /* insert product to existing quality */
    if (x->quality == root->quality)
    {
        newTimeRoot = insertTime(root->timeSubtree, x);             /* add product to quality's time subtree */
        root->timeSubtree = newTimeRoot;                            /* update time subtree root */
        root->subtreeSize++;                                        /* update subtree size */
        return root;
    }
    root->subtreeSize++;    /* update subtree size */
    root = balance(root);   /* balance the tree */
    return root;
}

/* returns the minimum product in tree (O(logn)) */
Product* minProduct(Product* root)
{
    Product* current = root;
    if (root == NULL) return NULL;
    
    /* go to the far left product */
    while (current->left != NULL) current = current->left;
    return current;
}

/* returns maximum product in tree (O(logn)) */
Product* maxProduct(Product* root)
{
    Product* current = root;
    if (root == NULL) return NULL;

    /* go to the far right */
    while (current->right != NULL) current = current->right;
    return current;
}

/* removes a product from time tree and returns a pointer to new root (O(logn)) */
Product* removeProductFromTime(Product* root, int time)
{
    Product *temp;

    /* base case */
    if (root == NULL) return NULL;

    /* search left subtree */
    if (root->time > time) root->left = removeProductFromTime(root->left, time);

    /* search in right subtree */
    else if (root->time < time) root->right = removeProductFromTime(root->right, time);

    /* if we found the product to remove */
    else
    {
        /* if it has one child or no children */
        if (root->left == NULL || root->right == NULL)
        {
            if (root->left == NULL) temp = root->right;
            else if (root->right == NULL) temp = root->left;

            /* check if product to delete is minimum quality */
            if (root->parent && root->parent->minQualityP->time == root->time) root->parent->minQualityP = NULL;

            /* update parent pointers */
            if (temp) temp->parent = root->parent;

            free(root);
            return temp;
        }
        /* if it has two children */
        temp = minProduct(root->right);                                 /* find successor */
        swapProduct(root, temp);                                        /* root = successor */
        root->right = removeProductFromTime(root->right, temp->time);   /* remove successor place product */
    }
    /* if it has only one node */
    if (root == NULL) return root;

    /* update height of current node and balance the tree */
    updateHeight(root);
    updateMinQuality(root);
    root->subtreeSize--;
    return balance(root);
}

/* removes a product from quality tree and returns new root (O(logn)) */
Product* removeProductFromQuality(Product* root, int time, int quality)
{
    Product* temp, *newTimeRoot;

    /* base case */
    if (root == NULL) return NULL;

    /* search left subtree */
    if (root->quality > quality) root->left = removeProductFromQuality(root->left, time, quality);

    /* search right subtree */
    else if (root->quality < quality) root->right = removeProductFromQuality(root->right, time, quality);

    /* found quality */
    else
    {
        /* remove product from time subtree (O(logn)) */
       newTimeRoot = NULL;
        if (root->timeSubtree != NULL) newTimeRoot = removeProductFromTime(root->timeSubtree, time);

        if (newTimeRoot == NULL)    /* remove quality node */
        {
            if (root->quality == quality) root->timeSubtree = NULL;

            /* if it has one child or no children */
            if (root->left == NULL || root->right == NULL)
            {
                if (root->left == NULL) temp = root->right;
                else if (root->right == NULL) temp = root->left;
                if (temp) temp->parent = root->parent;                          /* update parent pointers */
                free(root);
                root = NULL;
                return temp;
            }
            /* if it has two children */
            temp = minProduct(root->right);                                             /* find successor */
            swapProduct(root, temp);                                                    /* root = successor */
            root->right = removeProductFromQuality(root->right, -1, temp->quality);     /* remove successor place product */
        }
        root->timeSubtree = newTimeRoot;    /* update time subtree */
    }
    /* if it has only one node */
    if (root == NULL) return root;

    /* update height and subtree size of current node and balance the tree */
    updateHeight(root);
    root->subtreeSize--;
    return balance(root);
}

/* returns time subtree size of a quality type node */
int timeSubtreeSize(Product* qualityRoot)
{
    if (qualityRoot == NULL || qualityRoot->timeSubtree == NULL) return 0;
    return qualityRoot->timeSubtree->subtreeSize;
}

/* gets time root and returns the ith product (O(logn)) */
Product* findIthTime(Product* root, int i)
{
    int leftSize;
    
    if (root == NULL) return NULL;  /* base case */

    leftSize = (root->left ? root->left->subtreeSize : 0);

    /* if i is out of range */
    if (i < 1 || i > root->subtreeSize) return NULL;

    /* found i-th rank product */
    if (i == leftSize + 1) return root;

    /* check left subtree */
    else if (i <= leftSize) return findIthTime(root->left, i);

    /* check right subtree */
    else return findIthTime(root->right, i - leftSize - 1);
}

/* gets quality tree root and returns the i-th rank product (O(logn)) */
Product* findIthQuality(Product* root, int i)
{
    int leftSize;
    
    if (root == NULL) return NULL;   /* base case */

    leftSize = (root->left ? root->left->subtreeSize : 0);

    /* if i is out of range */
    if (i < 1 || i > root->subtreeSize) return NULL;

    /* ith product is in current node time subtree */
    if (i > leftSize && i <= leftSize + timeSubtreeSize(root))
    {
        return findIthTime(root->timeSubtree, i - leftSize);
    }

    /* ith product is in left subtree */
    else if (i <= leftSize) return findIthQuality(root->left, i);

    /* ith product is in right subtree */
    else return findIthQuality(root->right, i - leftSize - timeSubtreeSize(root));
}

/* returns if a products time is between time1 and time2 */
int isInRange(Product* x, int time1, int time2)
{
    if (x == NULL) return 0;
    return (x->time >= time1 && x->time <= time2);
}

/* find product by time or its successor if doesnt exists (O(logn)) */
Product* findTimeOrSuccessor(Product* root, int time)
{
    Product* successor = NULL;
    while (root != NULL)
    {
        if (root->time > time)
        {
            successor = root;
            root = root->left;
        }
        else if (root->time < time) root = root->right;
        else return root;   /* found time */
    }
    return successor;       /* time not found, return successor */
}

/* find product by time or predecessor if doesnt exists (O(logn)) */
Product* findTimeOrPredecessor(Product* root, int time)
{
    Product* predecessor = NULL;
    while (root != NULL)
    {
        if (root->time < time)
        {
            predecessor = root;
            root = root->right;
        }
        else if (root->time > time) root = root->left;
        else return root;       /* found time */
    }
    return predecessor;         /* time not found, return predecessor */
}

/* compare minimum product pointer between root and right subtree (for when root is left child) */
Product* minProductLeft(Product* root, Product* min, int time1, int time2)
{
    /* empty tree */
    if (root == NULL) return NULL;

    /* search left subtree */
    if (root->time > time1)
    {
        min = minProductLeft(root->left, min, time1, time2);
        if (root->right && isInRange(root->right, time1, time2)) min = minOfTwoProducts(root->right->minQualityP, min);
        if (isInRange(root, time1, time2)) min = minOfTwoProducts(root, min);
        return min;
    }
    /* search right subtree */
    else if (root->time < time1)
    {
        min = minProductLeft(root->right, min, time1, time2);
        return min;
    }
    /* found product */
    else
    {
        min = root;
        if (root->right && isInRange(root->right, time1, time2)) return minOfTwoProducts(min, root->right->minQualityP);
    }
    return min;
}

/* compare minimum product pointer between root and left subtree (for when root is right child) */
Product* minProductRight(Product* root, Product* min, int time1, int time2)
{
    /* empty tree */
    if (root == NULL) return NULL;

    /* search left subtree */
    if (root->time > time2)
    {
        min = minProductRight(root->left, min, time1, time2);
        return min;
    }
    /* search right subtree */
    else if (root->time < time2)
    {
        min = minProductRight(root->right, min, time1, time2);
        if (root->left && isInRange(root->left, time1, time2)) min = minOfTwoProducts(root->left->minQualityP, min);
        if (isInRange(root, time1, time2)) min = minOfTwoProducts(root, min);
        return min;
    }
    /* found product */
    else
    {
        min = root;
        if (root->left && isInRange(root->left, time1, time2)) return minOfTwoProducts(min, root->left->minQualityP);
    }
    return min;
}

/* returns minimum product between time1 and time2 */
Product* findMinQualityBetween(Product* root, int left, int right)
{
    Product* minLeft, *minRight;
    
    /* empty tree */
    if (root == NULL) return NULL;

    /* find minimum quality in range */
    minLeft = minProductLeft(root, minLeft, left, right);
    minRight = minProductRight(root, minRight, left, right);

    /* return minimum between left and right */
    if (minLeft == minRight) return minLeft;
    else return minOfTwoProducts(minLeft, minRight);
}

/* returns how many products are between time1 and time2 O(logn) */
int countProducts(Product* root, int time1, int time2)
{
    if (root == NULL) return 0;
    if (root->time < time1) return countProducts(root->right, time1, time2);
    if (root->time > time2) return countProducts(root->left, time1, time2);
    return 1 + countProducts(root->left, time1, time2) + countProducts(root->right, time1, time2);
}

/* update given time product's quality to new */
Product* updateToNewQuality(Product* root, int time, int newQuality)
{
    if (root->time > time)
    {
        root->left = updateToNewQuality(root->left, time, newQuality);
    }
    if (root->time < time)
    {
        root->right = updateToNewQuality(root->right, time, newQuality);
    }
    else if (root->time == time) root->quality = newQuality;

    return root;
}

int main()
{
    int current;

    DataStructure ds = Init(11); // initializes an empty data structure
    AddProduct(&ds, 4, 11); // Adds a product at time t=4 and quality q=11
    AddProduct(&ds, 6, 12); // Adds a product at time t=6 and quality q=12
    AddProduct(&ds, 2, 13); // Adds a product at time t=2 and quality q=13
    AddProduct(&ds, 1, 14); // Adds a product at time t=1 and quality q=14
    AddProduct(&ds, 3, 15); // Adds a product at time t=3 and quality q=15
    AddProduct(&ds, 5, 17); // Adds a product at time t=5 and quality q=17
    AddProduct(&ds, 7, 17); // Adds a product at time t=7 and quality q=17
    
    current = GetIthRankProduct(ds, 1); //The i=1 best product has time t=4 and quality q=11,returns 4
    printf("%d \n", current);
    
    current = GetIthRankProduct(ds, 2); //The i=2 best product has time t=6 and quality q=12,returns 6
    printf("%d \n", current);

    current = GetIthRankProduct(ds, 6); //The i=”6 best product” has time t=5 and quality q=17,returns 5
    printf("%d \n", current);
    
    current = GetIthRankProduct(ds, 7); //The i=”7 best product” has time t=7 and quality q=17,returns 7
    printf("%d \n", current);
    
    current = GetIthRankProductBetween(ds, 2, 6, 3); // looks at values with time {2,3,4,5,6} and returns the i=”3 best product” between them, which has time t=2.
    printf("%d \n", current);
    
    current = Exists(ds); // returns 1, since there exists a product with quality q=s=11
    printf("%d \n", current);
    
    RemoveProduct(&ds, 4); // removes product with time t=4 from the data structure
    current = Exists(ds); // returns 0, since there is no product with quality q=s=11
    printf("%d \n", current);
    
    return 0;
}