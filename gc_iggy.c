
#include "gc_iggy.h"
#include "gc_common.h"

struct IGGYHeader gc_parse_iggy(const char *filename)
{
    FILE *file = fopen(filename, "rb");
    struct IGGYHeader header = {0};

    if (!file)
    {
        gc_MessageBox("File Opening Error", MB_ICONERROR);
    }

    if (fread(&header, sizeof(header), 1, file) != 1)
    {
        gc_MessageBox("Header Read Error", MB_ICONERROR);
        fclose(file);
    }

    // Verifica la firma
    if (header.signature != IGGY_SIGNATURE)
    {
        fprintf(stderr, "Firma del file non valida\n");
        fclose(file);
    }

    // Stampa l'intestazione del file
    printf("Signature: 0x%" PRIx32 "\n", header.signature);
    if (header.plattform[1] == 64)
    {
        printf("Version: 64-bit\n");
    }
    else if (header.plattform[1] == 32)
    {
        printf("Version: 32-bit\n");
    }
    else
    {
        printf("Unknown Version: %" PRIu32 "\n", header.version);
    }

    // Stampa la piattaforma come esadecimale
    printf("Platform: ");
    for (int i = 0; i < 4; i++)
    {
        printf("%02X ", header.plattform[i]);
    }
    printf("\n");

    printf("Unknown 0x0C: %" PRIu32 "\n", header.unk_0C);
    printf("Number of subfiles: %" PRIu32 "\n", header.num_subfiles);

    // Leggi le voci dei sottotitoli
    size_t subfile_size = sizeof(struct IGGYSubFileEntry);
    struct IGGYSubFileEntry *subfiles = gc_malloc(header.num_subfiles * subfile_size);
    if (!subfiles)
    {
        gc_MessageBox("gc_malloc Error", MB_ICONERROR);
        fclose(file);
    }

    if (fread(subfiles, subfile_size, header.num_subfiles, file) != header.num_subfiles)
    {
        gc_MessageBox("Subfile Read Error", MB_ICONERROR);
        gc_free(subfiles);
        fclose(file);
    }

    // Stampa le voci dei sottotitoli
    for (uint32_t i = 0; i < header.num_subfiles; i++)
    {
        printf("Subfile %u:\n", i);
        printf("  ID: %" PRIu32 "\n", subfiles[i].id);
        printf("  Size: %" PRIu32 "\n", subfiles[i].size);
        printf("  Size2: %" PRIu32 "\n", subfiles[i].size2);
        printf("  Offset: %" PRIu32 "\n", subfiles[i].offset);
    }

    // Gestisci i dati Flash in base alla versione
    if (header.plattform[1] == 64)
    {
        // Logica per 64 bit
        struct IGGYFlashHeader64 flash_header;
        if (fread(&flash_header, sizeof(flash_header), 1, file) != 1)
        {
            gc_MessageBox("Flash Header (64-bit) Read Error", MB_ICONERROR);
            gc_free(subfiles);
            fclose(file);
        }

        // Stampa dell'intestazione Flash (64 bit)
        printf("\n64-bit Flash Header:\n");
        printf("Main Offset: %" PRIu64 "\n", flash_header.main_offset);
        printf("AS3 Section Offset: %" PRIu64 "\n", flash_header.as3_section_offset);
        printf("Unknown Offset: %" PRIu64 "\n", flash_header.unk_offset);
        printf("Unknown Offset2: %" PRIu64 "\n", flash_header.unk_offset2);
        printf("Unknown Offset3: %" PRIu64 "\n", flash_header.unk_offset3);
        printf("Unknown Offset4: %" PRIu64 "\n", flash_header.unk_offset4);
        printf("Unknown 0x30: %" PRIu64 "\n", flash_header.unk_30);
        printf("Width: %" PRIu32 "\n", flash_header.width);
        printf("Height: %" PRIu32 "\n", flash_header.height);
        printf("Unknown 0x40: %" PRIu32 "\n", flash_header.unk_40);
        printf("Unknown 0x44: %" PRIu32 "\n", flash_header.unk_44);
        printf("Unknown 0x48: %" PRIu32 "\n", flash_header.unk_48);
        printf("Unknown 0x4C: %" PRIu32 "\n", flash_header.unk_4C);
        printf("Names Offset: %" PRIu64 "\n", flash_header.names_offset);
        printf("Unknown Offset5: %" PRIu64 "\n", flash_header.unk_offset5);
        printf("Unknown 0x58: %f\n", flash_header.framerate);
        printf("Last Section Offset: %" PRIu64 "\n", flash_header.last_section_offset);
        printf("Unknown Offset6: %" PRIu64 "\n", flash_header.unk_offset6);
        printf("AS3 Code Offset: %" PRIu64 "\n", flash_header.as3_code_offset);
        printf("AS3 Names Offset: %" PRIu64 "\n", flash_header.as3_names_offset);
        printf("Unknown 0xA8: %" PRIu32 "\n", flash_header.unk_A8);
        printf("Unknown 0xAC: %" PRIu32 "\n", flash_header.unk_AC);
        printf("Unknown 0xB0: %" PRIu32 "\n", flash_header.unk_B0);
        printf("Unknown 0xB4: %" PRIu32 "\n", flash_header.unk_B4);
    }
    else if (header.plattform[1] == 32)
    {
        // Logica per 32 bit
        struct IGGYFlashHeader32 flash_header;
        if (fread(&flash_header, sizeof(flash_header), 1, file) != 1)
        {
            gc_MessageBox("Flash Header Read Error", MB_ICONERROR);
            gc_free(subfiles);
            fclose(file);
        }

        // Stampa dell'intestazione Flash (32 bit)
        printf("\n32-bit Flash Header:\n");
        printf("Main Offset: %" PRIu32 "\n", flash_header.main_offset);
        printf("AS3 Section Offset: %" PRIu32 "\n", flash_header.as3_section_offset);
        printf("Unknown Offset: %" PRIu32 "\n", flash_header.unk_offset);
        printf("Unknown Offset2: %" PRIu32 "\n", flash_header.unk_offset2);
        printf("Unknown Offset3: %" PRIu32 "\n", flash_header.unk_offset3);
        printf("Unknown Offset4: %" PRIu32 "\n", flash_header.unk_offset4);
        printf("Unknown 0x30: %" PRIu32 "\n", flash_header.unk_30);
        printf("Width: %" PRIu32 "\n", flash_header.width);
        printf("Height: %u\n", flash_header.height);
        printf("Framerate: %f\n", flash_header.framerate);
        printf("Unknown 0x44: %" PRIu32 "\n", flash_header.unk_44);
        printf("Unknown 0x48: %" PRIu32 "\n", flash_header.unk_48);
        printf("Unknown 0x4C: %" PRIu32 "\n", flash_header.unk_4C);
        printf("Names Offset: %" PRIu32 "\n", flash_header.names_offset);
        printf("Unknown Offset5: %" PRIu32 "\n", flash_header.unk_offset5);
        printf("Unknown 0x58: %" PRIu64 "\n", flash_header.unk_58);
        printf("Last Section Offset: %" PRIu32 "\n", flash_header.last_section_offset);
        printf("Unknown Offset6: %" PRIu32 "\n", flash_header.unk_offset6);
        printf("AS3 Code Offset: %" PRIu32 "\n", flash_header.as3_code_offset);
        printf("AS3 Names Offset: %" PRIu32 "\n", flash_header.as3_names_offset);
        printf("Unknown 0x70: %" PRIu32 "\n", flash_header.unk_70);
        printf("Unknown 0x74: %" PRIu32 "\n", flash_header.unk_74);
        printf("Unknown 0x78: %" PRIu32 "\n", flash_header.unk_78);
        printf("Unknown 0x7C: %" PRIu32 "\n", flash_header.unk_7C);
    }
    else
    {
        printf("ERROR - UNKNOWN VERSION, CAN'T PROCEED");
    }
    // Pulisci
    gc_free(subfiles);
    fclose(file);
    return header;
}
// Function to parse the subfiles from an IGGY file
int gc_parse_iggy_subfiles(const char *file_path, struct IGGYSubFileEntry *subfiles, unsigned int num_subfiles) {
    FILE *file = fopen(file_path, "rb");
    if (!file) {
        return -1;  // File open error
    }

    // Assume that the subfiles start right after the header.
    // We read the subfiles into the provided subfiles array.
    fseek(file, subfiles->offset, SEEK_SET);  // Skip header

    // Read the subfile entries into the subfiles array
    for (unsigned int i = 0; i < num_subfiles; i++) {
        if (fread(&subfiles[i], sizeof(struct IGGYSubFileEntry), 1, file) != 1) {
            fclose(file);
            return -2;  // Error reading subfiles
        }
    }

    fclose(file);
    return 0;  // Success
}
int gc_iggyDoXml(const char *file_path, const char *xml_output_path) {
    printf("Start processing IGGY file: %s\n", file_path);

    // Parse the IGGY file header
    struct IGGYHeader header = gc_parse_iggy(file_path);
    printf("Parsed IGGY header: version=%u, num_subfiles=%u\n", header.version, header.num_subfiles);

    gc_size subfile_size = sizeof(struct IGGYSubFileEntry);
    struct IGGYSubFileEntry *subfiles = gc_malloc(header.num_subfiles * subfile_size);
    if (!subfiles) {
        printf("Memory allocation failed for subfiles\n");
        return -2;  // Memory allocation failed
    }
    printf("Allocated memory for %u subfiles\n", header.num_subfiles);

    // Read subfiles data from the IGGY file
    if (gc_parse_iggy_subfiles(file_path, subfiles, header.num_subfiles) != 0) {
        gc_free(subfiles);
        printf("Error reading subfiles from IGGY file\n");
        return -3;  // Error reading subfiles
    }
    printf("Successfully read %u subfiles from IGGY file\n", header.num_subfiles);

    // Create the root XML node, representing the IGGY file
    XmlNode *root = gc_xml_create_node("IGGY", "Root of IGGY file");
    printf("Created root XML node: IGGY\n");

    // Add version information as XML child
    char version_str[32];
    snprintf(version_str, sizeof(version_str), "%u", header.version);
    XmlNode *version_node = gc_xml_create_node("Version", version_str);
    gc_xml_add_child(root, version_node);
    printf("Added Version node with value: %s\n", version_str);

    // Add subfiles to the XML
    XmlNode *subfiles_node = gc_xml_create_node("Subfiles", "");
    gc_xml_add_child(root, subfiles_node);
    printf("Added Subfiles node to XML\n");

    // Iterate over the subfiles and create XML nodes for each
    for (int i = 0; i < header.num_subfiles; ++i) {
        struct IGGYSubFileEntry subfile = subfiles[i];
        
        // Create an XML node for each subfile
        char subfile_id[32];
        snprintf(subfile_id, sizeof(subfile_id), "%u", subfile.id);
        XmlNode *subfile_node = gc_xml_create_node("Subfile", subfile_id);
        printf("Created Subfile node with ID: %s\n", subfile_id);
        
        // Add size and offset information to the subfile XML node
        char size_str[32];
        snprintf(size_str, sizeof(size_str), "Size: %u", subfile.size);
        XmlNode *size_node = gc_xml_create_node("Size", size_str);
        gc_xml_add_child(subfile_node, size_node);
        printf("Added Size node with value: %s\n", size_str);
        
        char offset_str[32];
        snprintf(offset_str, sizeof(offset_str), "Offset: %u", subfile.offset);
        XmlNode *offset_node = gc_xml_create_node("Offset", offset_str);
        gc_xml_add_child(subfile_node, offset_node);
        printf("Added Offset node with value: %s\n", offset_str);
        
        // Add the subfile node to the "Subfiles" node
        gc_xml_add_child(subfiles_node, subfile_node);
        printf("Added Subfile node to Subfiles list\n");
    }

    // Generate XML output to a file
    printf("Generating XML output to file: %s\n", xml_output_path);
    FILE *xml_file = fopen(xml_output_path, "w");
    if (!xml_file) {
        gc_free(subfiles);
        printf("Error opening XML output file: %s\n", xml_output_path);
        return -4;  // Error opening XML output file
    }

    gc_xml_write(xml_file, root);  // Assuming this function writes the XML to the file
    printf("Successfully wrote XML to file: %s\n", xml_output_path);

    // Clean up allocated memory for subfiles and close the file
    gc_free(subfiles);
    fclose(xml_file);
    printf("Cleaned up and closed XML file\n");

    return 0;  // Success
}
int gc_xmlDoIggy(const char *xml_data, const char *output_path) {
    printf("Start processing XML data: %s\n", xml_data);

    // Parse the XML data into an XmlNode structure
    XmlNode *root = gc_xml_parse(xml_data);
    if (!root) {
        printf("Error parsing XML data\n");
        return -1;  // Error parsing XML
    }
    printf("Parsed XML root node: %s\n", root->tag);

    // Extract version from the XML
    XmlNode *version_node = root->children;
    if (!version_node || strcmp(version_node->tag, "Version") != 0) {
        printf("Error: missing or malformed version information\n");
        return -2;  // Error: missing or malformed version information
    }
    unsigned int version = atoi(version_node->content);
    printf("Extracted version from XML: %u\n", version);

    // Create the IGGYHeader structure
    struct IGGYHeader header;
    header.version = version;
    header.num_subfiles = 0;  // Will be updated after processing subfiles
    printf("Created IGGYHeader: version=%u, num_subfiles=%u\n", header.version, header.num_subfiles);

    // Allocate memory for subfiles based on number of subfiles in the XML
    XmlNode *subfiles_node = root->children->next; // Should be "Subfiles" node
    if (!subfiles_node || strcmp(subfiles_node->tag, "Subfiles") != 0) {
        printf("Error: missing or malformed subfiles node\n");
        return -3;  // Error: missing or malformed subfiles node
    }
    printf("Found Subfiles node\n");

    // Allocate memory for the subfiles
    struct IGGYSubFileEntry *subfiles = gc_malloc(sizeof(struct IGGYSubFileEntry) * header.num_subfiles);
    if (!subfiles) {
        printf("Memory allocation failed for subfiles\n");
        return -4;  // Memory allocation failed
    }
    printf("Allocated memory for %u subfiles\n", header.num_subfiles);

    // Process each subfile node in the XML
    XmlNode *subfile_node = subfiles_node->children;
    int subfile_index = 0;
    while (subfile_node) {
        // Extract subfile ID
        unsigned int subfile_id = atoi(subfile_node->content);
        printf("Extracted Subfile ID: %u\n", subfile_id);

        // Extract size and offset from the XML nodes
        XmlNode *size_node = subfile_node->children;
        unsigned int subfile_size = 0;
        if (size_node && strcmp(size_node->tag, "Size") == 0) {
            subfile_size = atoi(size_node->content);
            printf("Extracted Subfile Size: %u\n", subfile_size);
        }

        XmlNode *offset_node = size_node->next;
        unsigned int subfile_offset = 0;
        if (offset_node && strcmp(offset_node->tag, "Offset") == 0) {
            subfile_offset = atoi(offset_node->content);
            printf("Extracted Subfile Offset: %u\n", subfile_offset);
        }

        // Store the subfile entry
        subfiles[subfile_index] = (struct IGGYSubFileEntry) {
            .id = subfile_id,
            .size = subfile_size,
            .offset = subfile_offset
        };
        subfile_index++;

        subfile_node = subfile_node->next;
    }
    printf("Processed %d subfiles\n", subfile_index);

    // After processing XML, write the reconstructed IGGY file
    printf("Writing reconstructed IGGY file to: %s\n", output_path);
    FILE *output_file = fopen(output_path, "wb");
    if (!output_file) {
        free(subfiles);
        printf("Error opening output file: %s\n", output_path);
        return -5;  // Error opening output file
    }

    // Write the IGGY header
    fwrite(&header, sizeof(header), 1, output_file);
    printf("Wrote IGGY header to file\n");

    // Write the subfiles
    fwrite(subfiles, sizeof(struct IGGYSubFileEntry), header.num_subfiles, output_file);
    printf("Wrote %u subfiles to file\n", header.num_subfiles);

    // Clean up
    free(subfiles);
    fclose(output_file);
    printf("Cleaned up and closed IGGY file\n");

    return 0; // Success
}
