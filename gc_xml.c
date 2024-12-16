#include "gc_xml.h"
#include "gc_memory.h"

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

// Function to parse an XML string and return the root node
XmlNode *xml_parse(const char *xml) {
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

// Function to print XML structure (for debugging)
void gc_xml_print(XmlNode *node, int indent) {
    if (!node) return;

    for (int i = 0; i < indent; i++) {
        printf("  "); // Indentation
    }

    printf("<%s>%s</%s>\n", node->tag, node->content ? node->content : "", node->tag);

    // Print children nodes
    gc_xml_print(node->children, indent + 1);

    // Print next sibling nodes
    gc_xml_print(node->next, indent);
}
