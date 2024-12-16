#ifndef XML_H
#define XML_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gc_common.h"

// Define a structure for XML tags (elements)
typedef struct XmlNode {
    char *tag;                // Tag name (e.g., <tag>)
    char *content;            // Content inside the tag (e.g., value)
    struct XmlNode *next;     // Next sibling in the hierarchy (for the same parent)
    struct XmlNode *children; // Child nodes (for nested elements)
} XmlNode;

// Function to create a new XML node
XmlNode *gc_xml_create_node(const char *tag, const char *content);

// Function to add a child node to a parent
void gc_xml_add_child(XmlNode *parent, XmlNode *child);

// Function to parse an XML string and return the root node
XmlNode *gc_xml_parse(const char *xml);

// Function to free an XML node and its children
void gc_xml_free(XmlNode *node);

// Function to print XML structure (for debugging)
void gc_xml_print(XmlNode *node, int indent);

#endif // XML_H
