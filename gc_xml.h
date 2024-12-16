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

XmlNode *gc_xml_create_node(const char *tag, const char *content);
XmlNode *gc_xml_parse(const char *xml);
void gc_xml_add_child(XmlNode *parent, XmlNode *child);
void gc_xml_free(XmlNode *node);
void gc_xml_write_node(FILE *file, XmlNode *node);
int gc_xml_write(FILE *file, XmlNode *root);
#endif // XML_H
