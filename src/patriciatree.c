#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "roaring.c"

char *__ao(uint32_t addr, char *buf, u_short bufLen)
{
    char *cp, *retStr;
    u_int byte;
    int n;
    cp = &buf[bufLen];
    *--cp = '\0';
    n = 4;
    do
    {
        byte = addr & 0xff;
        *--cp = byte % 10 + '0';
        byte /= 10;
        if (byte > 0)
        {
            *--cp = byte % 10 + '0';
            byte /= 10;
            if (byte > 0)
                *--cp = byte + '0';
        }
        *--cp = '.';
        addr >>= 8;
    } while (--n > 0);


    retStr = (char *)(cp + 1); // to lower case
    return (retStr);
}

static char buf[sizeof "ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff"];
char *ao(uint32_t addr) { return (__ao(addr, buf, sizeof(buf))); }

struct Result {
    uint32_t ip;
    roaring_bitmap_t *ports;
};

struct Node {
    int leaf;
    struct Node *left;
    struct Node *right;
    roaring_bitmap_t *ports;
};
 
struct Node* createNode() {
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
    if (newNode != NULL) {
        newNode->leaf = 0;
        newNode->left = NULL;
        newNode->right = NULL;
        newNode->ports = roaring_bitmap_create();;
    }
    return newNode;
}

int insert(struct Node* root, uint32_t ip,int total) {
    printf("inserisco %s\n",ao(ntohl(ip)));
    struct Node* currentNode = root;
    for (int i = 31; i >= 0; i--) {
        int bit = (ip >> i) & 1;
        if (bit == 0) {
            if (currentNode->left == NULL) {
                currentNode->left = createNode();
            }
            currentNode = currentNode->left;
        } else {
            if (currentNode->right == NULL) {
                currentNode->right = createNode();
            }
            currentNode = currentNode->right;
        }
    }
    currentNode->leaf = 1;
    return ++total;
}

roaring_bitmap_t* search(struct Node* root, uint32_t ip) {
    struct Node* currentNode = root;
    for (int i = 31; i >= 0; i--) {
        int bit = (ip >> i) & 1;
        if (bit == 0) {
            if (currentNode->left == NULL) {
                return NULL; 
            }
            currentNode = currentNode->left;
        } else {
            if (currentNode->right == NULL) {
                return NULL; 
            }
            currentNode = currentNode->right;
        }
    }
    if (currentNode->leaf) {
        return currentNode->ports; 
    } else {
        return NULL; 
    }
}

//void traverseTree(struct Node* root) {
//    struct Node* stack[32]; // Stack di nodi
//    int top = -1;
//
//    struct Node* current = root;
//
//    while (current || top >= 0) {
//
//        while (current) {
//            stack[++top] = current;
//            current = current->left;
//        }
//
//        current = stack[top--];
//
//        // Visita il nodo corrente (foglia)
//        if (current->leaf) {
//            //printf("Indirizzo IP: %u.%u.%u.%u, Porta: %d\n",
//            //       (current->port >> 24) & 0xFF, (current->port >> 16) & 0xFF, 
//            //       (current->port >> 8) & 0xFF, current->port & 0xFF, current->port);
//        }
//
//        current = current->right;
//    }
//}

bool iter(uint32_t value, void* p)
{
    printf(" %u ",value);
    return true;
}

int z = 0;
void traverse(struct Node* node, uint32_t ip, int level, struct Result** res) {
    if (node == NULL) {
        return;
    }

    if (node->leaf) {
        res[z] = malloc(sizeof(struct Result));
        res[z]->ip = ip;
        res[z]->ports = node->ports;
        z = z+1;
    }

    traverse(node->left, ip << 1, level + 1, res);
    traverse(node->right, (ip << 1) | 1, level + 1, res);
}

struct Result** traverseTree(struct Node* root,int total) {
    z = 0;
    struct Result** res = calloc(total,sizeof(struct Result*));   
    traverse(root, 0, 0, res);
    return res;
}

//int main() {
//    struct Node* root = createNode();
//
//    insert(root, 0xC0A80001); // 192.168.0.1
//    insert(root, 0xC0A80002); // 192.168.0.2
//    insert(root, 0xC0A80101); // 192.168.1.1
//    
//    roaring_bitmap_t* a = search(root, 0xC0A80001);
//    roaring_bitmap_t* b = search(root, 0xC0A80002);
//    roaring_bitmap_t* o = search(root, 0xC0A80101);
//    roaring_bitmap_t* d = search(root, 0xC0A88101);
//
//    if (d == NULL && a != NULL && b != NULL && o != NULL) {
//        printf("funziona\n");
//    }
//    roaring_bitmap_add(a, 1234);
//    roaring_bitmap_add(b, 1264);
//    roaring_bitmap_add(o, 1294);
//
//    roaring_bitmap_add(a, 8239);
//    roaring_bitmap_add(b, 9236);
//    roaring_bitmap_add(o, 2634);
//
//    roaring_bitmap_add(a, 4345);
//    roaring_bitmap_add(b, 4234);
//    roaring_bitmap_add(o, 5234);
//
//    roaring_bitmap_add(a, 9999);
//
//    struct Result** susu = traverseTree(root);
//         
//    for (int i=0; i<total; i++) {
//        printf("ip: %s\n",intoa(susu[i]->ip));
//        roaring_iterate(susu[i]->ports, iter, NULL);
//    }
//
//    //    printf("%u\n",res[i]->ip);
//
//    return 0;
//}
