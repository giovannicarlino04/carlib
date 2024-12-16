#include "gc_xml.h"
#include "gc_memory.h"
#include "gc_common.h"
#include "gc_debug.h"

// Function to create a new XML node
XmlNode *gc_xml_create_node(const char *tag, const char *content) {
    XmlNode *node = (XmlNode *)malloc(sizeof(XmlNode));
    if (!node) {
        return NULL;
    }
    node->tag = strdup(tag);        // Copy tag name
    node->content = strdup(content); // Copy content inside the tag
    node->next = NULL;              // No sibling yet
    node->children = NULL;          // No children yet
    return node;
}

// Function to add a child node to a parent
void gc_xml_add_child(XmlNode *parent, XmlNode *child) {
    if (parent == NULL || child == NULL) {
        return;
    }

    if (parent->children == NULL) {
        parent->children = child;
    } else {
        XmlNode *current = parent->children;
        while (current->next) {
            current = current->next;
        }
        current->next = child;
    }
}

// Simple function to skip over white spaces
void skip_whitespace(const char **xml) {
    while (**xml && (**xml == ' ' || **xml == '\n' || **xml == '\t' || **xml == '\r')) {
        (*xml)++;
    }
}

// Function to parse a single tag (both opening and closing tags)
int parse_tag(const char **xml, char **tag) {
    skip_whitespace(xml);
    if (**xml != '<') {
        return 0;  // Error: expected '<'
    }

    (*xml)++; // Skip the '<'

    // Parse the tag name
    const char *start = *xml;
    while (**xml && **xml != ' ' && **xml != '>' && **xml != '/') {
        (*xml)++;
    }
    size_t tag_length = *xml - start;
    *tag = gc_strndup(start, tag_length);

    skip_whitespace(xml);

    // Check for closing '>'
    if (**xml == '>') {
        (*xml)++;
        return 1;
    } else if (**xml == '/') {
        (*xml)++;  // Self-closing tag (e.g., <tag/>)
        if (**xml == '>') {
            (*xml)++;
            return 1;
        }
    }

    return 0;  // Error
}
// Helper function to write an XML node and its children recursively
void gc_xml_write_node(FILE *file, XmlNode *node) {
    if (!node) {
        return;
    }

    // Write the opening tag
    fprintf(file, "<%s>", node->tag);

    // Write the content of the node (if any)
    if (node->content) {
        fprintf(file, "%s", node->content);
    }

    // If the node has children, write them recursively
    if (node->children) {
        gc_xml_write_node(file, node->children);
    }

    // Write the closing tag
    fprintf(file, "</%s>", node->tag);

    // Move to the next sibling node
    gc_xml_write_node(file, node->next);
}

// Function to write the entire XML tree to a file
int gc_xml_write(FILE *file, XmlNode *root) {
    if (!file) {
        return -1;  // Error opening file
    }

    // Write the XML declaration (optional, can be added if needed)
    fprintf(file, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");

    // Write the root node and its children
    gc_xml_write_node(file, root);

    // Close the file
    fclose(file);
    return 0;  // Success
}

// Function to parse an XML string and return the root node
XmlNode *gc_xml_parse(const char *xml) {
    XmlNode *root = NULL;
    XmlNode *current_parent = NULL;

    while (*xml) {
        char *tag = NULL;
        char *content = NULL;

        // Parse opening tag
        if (parse_tag(&xml, &tag)) {
            XmlNode *node = gc_xml_create_node(tag, content);
            gc_free(tag);
            gc_free(content);

            if (root == NULL) {
                root = node; // The first node becomes the root
            }

            if (current_parent) {
                gc_xml_add_child(current_parent, node); // Add to the parent
            }

            current_parent = node; // Set current parent to the new node
        }

        // Handle closing tag (to go back up the tree)
        if (*xml == '<' && *(xml + 1) == '/') {
            xml++; // Skip the '<'
            xml++; // Skip the '/'
            skip_whitespace(&xml);

            // Parse the closing tag
            char *tag = NULL;
            if (parse_tag(&xml, &tag)) {
                free(tag);
                free(content);
                current_parent = NULL; // Return to the parent node
            }
        }

        skip_whitespace(&xml);
    }

    return root;
}

// Function to free an XML node and its children
void gc_xml_free(XmlNode *node) {
    if (node) {
        gc_free(node->tag);
        gc_free(node->content);
        XmlNode *child = node->children;
        while (child) {
            XmlNode *next = child->next;
            gc_xml_free(child);
            child = next;
        }
        gc_free(node);
    }
}
