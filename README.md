Octree
======

Custom Octree implementation.

# Description

This Octree allows internal nodes (or non-leaf nodes) hold references to elements (e.g. holding references to particles that, due to their size, do not fit in child nodes). A single linked list is used and shared between all octree nodes so each internal node is able to retrieve (in a single iteration) not only its elements but also those of its child nodes. To achieve this, each child node has its list nested in its parent list by means of special linked nodes. These linked nodes act as list separators, denoting the start and the end of the current node's list.
