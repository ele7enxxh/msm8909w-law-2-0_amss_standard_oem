package fdumpParser;
use strict;
use Data::Dumper;

# Debug                           
my $debug_mode      =    0;   

my @entries;
my %enumHashList    =   ();

# The below are required to store and dump structures
my @elements        =   ();
my %data_struct     =   ();
my %recordType      =   ();

# Node types and corresponding interesting fields
# type_decl         = name, type
# enumeral_type     = name, csts
# tree_list         = purp, valu, chan
# const_decl        = cnst
# integer_cst       = low
# identifier_node   = strg
# union_type        = name, flds, tag
# record_type       = name, flds, tag
# field_decl        = name, type, bitfield, size, bpos, chan
# pointer_type      = name, ptd
# array_type        = elts, domn
# integer_type      = name, size, sign, max

# Special parsing for node type "identifier_node" as literal names can have spaces and we
# can't form (name,value) pairs as we did for other node types
# Hence the logic here is
# 1. First split on spaces
# 2. Parse through the resulting array list
# 3. Form a string which is between "strg:" field and "lngt:" field
sub parse_identifier_node
{
    my $sourceString = shift;
    my @splitString;
    my $literalName = "";
    my $length;
    my $index;
    my $record      =   0;
    
    @splitString    = split(" ",$sourceString);
    
    $length         = @splitString;
    
    for($index=0 ; $index<$length ; $index++)
    {
        # Check if we reached the end point => "lngt:"
        if($record == 1 && $splitString[$index] =~ m/:/)
        {
            # Reached the end, need to stop parsing the source string
            # Before returning, remove the last character as we will have an additional " "
            $literalName    =   substr($literalName,0,length($literalName)-1);
            
            last;   # Break from the for loop and return the string
        }
        
        if($record == 1)
        {
            # If there are multiple literal parts, concatenate them separated by a " "
            $literalName .= $splitString[$index]." ";
        }
        
        # Check if we reached the start point => "strg:"
        if($splitString[$index] =~ m/:/)
        {
            $record = 1;
            next;       # Continue, don't execute the remaining part of the for loop
        }
    }
    
    return $literalName;
}

# Function to get field values from the string
# Input parameter1      Source String
# Return hashValue:     (fieldName, fieldValue) pairs
sub parse
{
    my $index;
    my $parseString = shift;
    my %returnHash;
    my @splitNodeItems;
    my $length;
    my $fieldName;
    my $fieldValue;
    
    #print "Line: ", $parseString,"\n";
    
    # Process the created string
    # Split the source string on space and colen
    # Hence we will have any array in which the first item will be the name of the field and the second item will be the value
    # If there are odd number of elements in the array we have an unexpected field/value, log it and skip the current parse
    # We might still have to eliminate any ":" or "@" from the field name, field value before filling the hash
    @splitNodeItems = split(/ +: +| +/,$parseString);
    
    $length = @splitNodeItems;
    
    if(0 != $length %2)
    {
        # Array length is not even hence we might have unexpected field in the -fdump-translation-unit dump
        # Log the error, skip the current parse and continue parsing the next node
        print "ERROR: Unexpected field name/value in the dump\nString: ",@splitNodeItems,"\n";
        $returnHash{name} = "NULL";
        
        return %returnHash;
    }
    
    # Store node_name
    $returnHash{node_name} = $splitNodeItems[1];
    
    # All even entries of the array will be fieldNames and odd entries will be fieldValues
    
    # Go through all the items - Start with the 2nd pair of (name,pair) as the 1st pair is just the nodeId
    # and nodeName in which we are not interested
    for($index=2 ; $index<$length ; $index+=2)
    {
        $fieldName              = $splitNodeItems[$index];
        $fieldValue             = $splitNodeItems[$index+1];
        
        # Make sure there is no ":" in the fieldName or "@" in the fieldValue
        $fieldName              =~ s/://g;
        $fieldValue             =~ s/@//g;
        
        $returnHash{$fieldName} = $fieldValue;
        
        #print "Name: ",$fieldName," Value: ",$fieldValue,"\n";
    }
    
    return %returnHash;
}

sub storeEntries
{
    my $fileName        =   shift;   
    my $index = 0;
    my @splitNodeItems;
    my %lineHash = {};
    my $line;
    my $node;
    my $nodeId;
    my $strg;
    
    open(inputFile, $fileName) or die "$!";
    
    # Read each line and fill stats structure
    while ($line=<inputFile>)
    {
        if(substr($line,0,1) eq "@")
        {
            #print $node,"\n";
            
            # Process the created string
            @splitNodeItems = split(" ",$node);
            
            # Store the nodeId
            $nodeId = substr($node,0,1);
            
            # Call the function parse() inside known node types as there are unwanted unpaired fieldnames
            # in certain node types (like function_decl which we don't want to parse)
            
            # Parse only interesting node types
            if($splitNodeItems[1] eq "type_decl" || $splitNodeItems[1] eq "integer_cst" || $splitNodeItems[1] eq "integer_cst" || $splitNodeItems[1] eq "enumeral_type" || $splitNodeItems[1] eq "tree_list" || $splitNodeItems[1] eq "const_decl" || $splitNodeItems[1] eq "record_type" || $splitNodeItems[1] eq "union_type" || $splitNodeItems[1] eq "integer_type" || $splitNodeItems[1] eq "field_decl" || $splitNodeItems[1] eq "array_type" || $splitNodeItems[1] eq "pointer_type")
            {
                # Call the function which will parse all the (fieldName,fieldValue) pairs and return a hash
                %lineHash           =   parse($node);
                
                # Clone the returned hash and push into the array @entries
                push(@entries,{%lineHash});
            }
            elsif($splitNodeItems[1] eq "identifier_node")
            {
                # Call the function which will parse all the (fieldName,fieldValue) pairs and return a hash
                #%lineHash          = parse($node);
                #$strg              = $lineHash{strg};
                
                $strg               = parse_identifier_node($node);
                
                #$entries[$index]   = {node_name => 'identifier_node', strg => $strg};
                #$index++;
                push(@entries,{node_name => 'identifier_node', strg => $strg});
            }
            elsif($splitNodeItems[1] eq "void_type")
            {   
                #$entries[$index] = {node_name => 'void_type'};
                #$index++;
                
                push(@entries,{node_name => 'void_type'});
            }
            elsif($splitNodeItems[1] eq "function_type")
            {
                #$entries[$index] = {node_name => 'function_type'};
                #$index++;
                
                push(@entries,{node_name => 'function_type'});
            }
            else
            {
                #$entries[$index] = {node_name => 'NULL'};
                #$index++;
                
                push(@entries,{node_name => 'NULL'});
            }
            
            # Reset the string
            $node = "";
            
            $index++;
        }
        
        # Remove the newline character at the end
        chomp($line);
        
        $node .= $line;
    }
    
    close(inputFile);
    
    # Return the number of entries recorded
    if($debug_mode == 1)
    {
        open (my $ofd, ">>entryDump.txt");
        for (my $ii=0; $ii<=$#entries; $ii++)
        {
             print $ofd "[",$ii,"], entry:",Dumper(\$entries[$ii]),"\n";
        }
        close ($ofd);
    }
    return @entries;
}

sub parse_typedef
{
    my $nodeId = shift;
    my $type_node;
    my $type_name;
    my $identifier_node;
    my $identifier_node_strg;
    my $integer_type_sign;
    my $integer_cst;
    my $integer_cst_low;
    my $integer_type;
    my $type_decl;
    my $pointer_type;
    my $type_definition_name;
    my $integer_size;
    my $integer_name;
    my $union_type;
    my $union_name;
    my $record_type;
    my $structure_name;
    
    # Initially $entries[$nodeId] is pointing to node "type_decl"
    # From this we extract the type name and other details
    
    $type_node                  =   $entries[$nodeId]{type};
    $type_name                  =   $entries[$type_node]{node_name};
    
    # Name
    $identifier_node            =   $entries[$nodeId]{name};
    $identifier_node_strg       =   $entries[$identifier_node]{strg};
    $type_definition_name       =   $identifier_node_strg;
            
    # If it is primitive data type then note the sign and the size.
    if($type_name eq "integer_type")
    {
        $integer_type = $type_node;
        
        # Now $type_node is pointing to "integer_type", we now need to extract
        # the identifier name, sign and size of the integer type
        
        # Name is extracted outside as it is common for all types
        
        # Sign
        $integer_type_sign      =   $entries[$integer_type]{sign};

        # Size
        $integer_cst            =   $entries[$integer_type]{size};
        $integer_cst_low        =   $entries[$integer_cst]{low};
        $integer_size           =   $integer_cst_low;
        
        print testFile "typedef ",$type_definition_name," ",$type_name," Sign: ",$integer_type_sign," Size: ",$integer_size,"\n\n";
    }
    elsif($type_name eq "pointer_type")
    {
        # Be careful here, this is kind of a recursive call
        # A pointer is pointing to another node which is nothing but a type_node
        # Using the type_node we get the type_name
        # type_name can be void, function or other types
        # The variables type_node and type_name are overwritten
        
        $type_node              =   $entries[$type_node]{ptd};
        $type_name              =   $entries[$type_node]{node_name};
        
        if($type_name eq "void_type")
        {
            #print "typedef ", $type_definition_name," Pointing to type:void\n\n";
            
            print testFile "typedef ", $type_definition_name," Pointing to type:void\n\n";
        }
        elsif($type_name eq "function_type")
        {
            #print "typedef Pointer to function Name: ",$type_definition_name,"\n\n";
            
            print testFile "typedef Pointer to function Name: ",$type_definition_name,"\n\n";
        }
        elsif($type_name eq "integer_type")
        {
            $integer_type                   =   $type_node;
                
            $type_decl                      =   $entries[$integer_type]{name};
            $identifier_node                =   $entries[$type_decl]{name};
            $identifier_node_strg           =   $entries[$identifier_node]{strg};
            $integer_name                   =   $identifier_node_strg;
            
            print testFile "typedef ",$type_definition_name,"Pointer to Type: ",$integer_name,"\n\n";
        }
        elsif($type_name eq "union_type")
        {
            $union_type                     =   $type_node;
            
            $type_decl                      =   $entries[$union_type]{name};
            $identifier_node                =   $entries[$type_decl]{name};
            $identifier_node_strg           =   $entries[$identifier_node]{strg};
            $union_name                     =   $identifier_node_strg;
            
            print testFile "typedef ",$identifier_node_strg,"Pointer to Union Type Name: ",$union_name,"\n\n";
        }
        else
        {   
            # Only thing left is struct
            
            $record_type                    =   $type_node;
            
            $type_decl                      =   $entries[$record_type]{name};
            $identifier_node                =   $entries[$type_decl]{name};
            $identifier_node_strg           =   $entries[$identifier_node]{strg};
            $structure_name                 =   $identifier_node_strg;
            
            print testFile "typedef ",$type_definition_name,"Pointer to Struct Type Name: ",$structure_name,"\n\n";
        }
    }
    elsif($type_name eq "record_type")
    {
        print testFile "typedef ", $type_definition_name," Type: struct\n\n";
    }
    elsif($type_name eq "union_type")
    {
        print testFile "typedef ", $type_definition_name," Type: union\n\n";
    }
    else
    {
        print testFile "typedef ", $type_definition_name," ",$type_name,"\n\n";
    }
}

# For enumerators which are unsigned, the value stored by compiler for 0xFFFFFFFF and -1 is the same
# Hence we need to distinguish between them.
# If we have a unsigned integer and its value is negative then call this function
# with the first parameter as the negative value and the second parameter as the size of the value in bits
sub getUnsignedValue
{
    my $value       =   shift;
    my $totalBits   =   shift;
    
    return ((2 ** $totalBits)+$value);
}

sub parse_enumerator
{
    my $nodeId                                      =   shift;
    my %localEnumList                               =   ();
    my $type_decl;
    my $identifier_node;
    my $identifier_node_strg;
    my $enumerator_member_name;
    my $const_decl;
    my $integer_cst;
    my $integer_cst_low;
    my $enumerator_value;
    my $enumerator_size;
    my $enumerator_sign;
    my $enumerator_name;
    
    #open(my $fd,">>enumDump.txt");
    
    # Handle primitive enumeration data type
    
    # Get the enumerator name
    $type_decl                                      =   $entries[$nodeId]{name};
    $identifier_node                                =   $entries[$type_decl]{name};
    $identifier_node_strg                           =   $entries[$identifier_node]{strg};
    $enumerator_name                                =   $identifier_node_strg;
    
    #print $fd "Enumerator => ",$enumerator_name,"\n";
    
    # Go through the enumerator
    my $parse                                       =   1;
    my $tree_list                                   =   $entries[$nodeId]{csts};  # Pointing to first tree_list
    
    while($parse == 1)
    {
        # Enumerator member name
        $identifier_node                            =   $entries[$tree_list]{purp};
        $identifier_node_strg                       =   $entries[$identifier_node]{strg};
        $enumerator_member_name                     =   $identifier_node_strg;
            
        # To find the value of the enumerator   
        $const_decl                                 =   $entries[$tree_list]{valu};
        $integer_cst                                =   $entries[$const_decl]{cnst};
        $integer_cst_low                            =   $entries[$integer_cst]{low};
        $enumerator_value                           =   $integer_cst_low;
        
        # Get size of the enumerator
        $integer_cst                                =   $entries[$nodeId]{size};
        $integer_cst_low                            =   $entries[$integer_cst]{low};
        $enumerator_size                            =   $integer_cst_low;
        
        # Get sign of the enumerator
        $enumerator_sign                            =   $entries[$nodeId]{sign};
        
        if($enumerator_value < 0 && $enumerator_sign eq "unsigned")
        {
            $enumerator_value   =   getUnsignedValue($enumerator_value,$enumerator_size);
        }
        
        # Store the enumerator name and value in the enum_hash
        #$enum_hash{$enumerator_member_name}         =   $enumerator_value;
        
        # Store it in the master list
        $localEnumList{$enumerator_member_name}     =   $enumerator_value;
        
        #print $fd $enumerator_member_name," ",$enumerator_value,"\n";
        
        # Find the next node in the enumerator tree
        if($entries[$tree_list]{chan} != "")
        {
            # Continue parsing
            $tree_list = $entries[$tree_list]{chan};
        }
        else
        {
            $parse = 0;
        }
    }
    
    $enumHashList{$enumerator_name}     =   {%localEnumList};
    
    #print $fd "\n";
    
    #close($fd);
}

sub parse_struct_union
{
    my $nodeId                  =   shift;
    my $type_decl;
    my $identifier_node;
    my $identifier_node_strg;
    my $struct_union_name;
    my $struct_union_type;
    my $node_name;
    my $field_member_type_name;
    my $type_node;
    my $type_name;
    my $integer_type;
    my $field_decl_bitfield;
    my $integer_cst;
    my $integer_cst_low;
    my $integer_size;
    my $integer_name;
    my $integer_type_sign;
    my $bitfield_length;
    my $bitfield_starting_bit_position;
    my $array_type;
    my $array_length;
    my $array_type_name;
    my $integer_type_size;
    my $enumeral_type;
    my $enumeration_type_name;
    my $enumerator_size;
    my $array_type_2;
    my $array_length_2;
    my $array_name;
    my $struct_size;
    my $union_size;
    my $pointer_type;
    my $function_name;
    my $pointing_to_name;
    my $record_type;
    my $structure_name;
    my $union_type;
    my $union_name;
    my $temp;
    my $enumerator_name;
    my $fd;
    
    # Below are required to record structure members - name is already present and can be used directly
    my $dump_type               =   "BYTE";     #   By default, only for struct and union it will change to "SUBREC"
    my $dump_unit_size          =   0;          #   This will have the size of the variable in bytes in case of primitive types
                                                #   This will have the name of the sub record in case of structures an unions
    my $dump_array_size         =   1;          #   This changes only for arrays
    my $dump_memberType         =   "default";
    my $dump_display_typeName   =   "";         #   This stores the name of types like uint16, uint32, int8, non primitive data types etc
    my $dump_array_dimension;                   #   This will have the below values
                                                #   0 - Simple data type
                                                #   1 - 1-D array
                                                #   2 - 2-D array
    my $sizeOfBitFieldBoundary  =   0;
    my $padBits                 =   0;
    my $padCounter              =   0;
    
    # Handle structures and unions
    
    if($debug_mode == 1)
    {
       open($fd,">>structDump.txt");
    }
    
    # Get the structure/union name
    $type_decl                  =   $entries[$nodeId]{name};
    $identifier_node            =   $entries[$type_decl]{name};
    $identifier_node_strg       =   $entries[$identifier_node]{strg};
    $struct_union_name          =   $identifier_node_strg;
    
    # entries[$nodeId] is pointing either to a union_type or record_type
    $node_name                  =   $entries[$nodeId]{node_name};
    
    if($node_name eq 'record_type')
    {
        print testFile "Structure => ",$struct_union_name,"\n";
        $struct_union_type      =   "struct";
    }
    elsif($node_name eq 'union_type')
    {
        print testFile "Union => ",$struct_union_name,"\n";
        $struct_union_type      =   "union";
    }
    
    if ($debug_mode)
    {
      print $fd $struct_union_type,"=>",$struct_union_name, "\n";
    }

    # Go through the structure
    my $parse                   =   1;
    my $field_decl              =   $entries[$nodeId]{flds};  # Pointing to first field declaration
    
    # Reset elements array
    @elements = ();
    
    while($parse == 1)
    {
        $node_name              =   $entries[$field_decl]{node_name};
        if ($debug_mode)
        {
            print $fd "\t\tfield:",$field_decl," node_name=",$node_name;
        }
        # The last member will be pointing to node of type "type_decl" at this point stop parsing
        # Go through the fields of the structure only if the node type is "field_decl"
        if($node_name eq "field_decl")
        {
            # Get the name of the field
            $identifier_node                    =   $entries[$field_decl]{name};
            $identifier_node_strg               =   $entries[$identifier_node]{strg};
            $field_member_type_name             =   $identifier_node_strg;
            
            #print $entries[$entries[$field_decl]{name}]{strg}," ";
            #print testFile "Name: ",$entries[$entries[$field_decl]{name}]{strg}," ";
            print testFile "Name: ",$field_member_type_name," ";
            
            # Now get the field_type
            # field_type node are named as "type_node"
            # "type_node" can be
            #                   1. integer_type
            #                   2. array_type
            #                   3. pointer_type
            #                   4. enumeral_type
            #                   5. record_type
            
            $type_node                          =   $entries[$field_decl]{type};
            $type_name                          =   $entries[$type_node]{node_name};
            
            if($type_name eq "integer_type")
            {
                $integer_type                           =   $type_node;
                $field_decl_bitfield                    =   $entries[$field_decl]{bitfield};
                    
                # Get the size of the type  
                $integer_cst                            =   $entries[$integer_type]{size};
                $integer_cst_low                        =   $entries[$integer_cst]{low};
                $integer_size                           =   $integer_cst_low;
                
                # Check if its a bitfield
                if($field_decl_bitfield eq "")
                {
                    # Get the type name
                    # type name can be int, unsigned int etc
                    $type_decl                          =   $entries[$integer_type]{name};
                    $identifier_node                    =   $entries[$type_decl]{name};
                    $identifier_node_strg               =   $entries[$identifier_node]{strg};
                    $integer_name                       =   $identifier_node_strg;
                    
                    # Get the sign of the type
                    $integer_type_sign                  =   $entries[$integer_type]{sign};
                    
                    print testFile "Type: ",$integer_name," Size: ",$integer_size," Sign: ",$integer_type_sign;
                    
                    # For dumping
                    $dump_unit_size                     =   ($integer_size/8);
                    $dump_memberType                    =   "default";
                    $dump_type                          =   "BYTE";
                    $dump_array_size                    =   1;
                    $dump_display_typeName              =   $integer_name;
                    $dump_array_dimension               =   0;
                }
                else
                {
                    # Get the sign of the type
                    $integer_type_sign                  =   $entries[$integer_type]{sign};
                    
                    # Note: The length and starting bit position of the bitfield can be accessed
                    # from the "field_decl" node
                    
                    # Get the length of the bitfield
                    $integer_cst                        =   $entries[$field_decl]{size};
                    $integer_cst_low                    =   $entries[$integer_cst]{low};
                    $bitfield_length                    =   $integer_cst_low;
                    
                    # Get the bitfield starting bit position
                    $integer_cst                        =   $entries[$field_decl]{bpos};
                    $integer_cst_low                    =   $entries[$integer_cst]{low};
                    $bitfield_starting_bit_position     =   $integer_cst_low;
                    
                    print testFile "Type: integer_type Bitfield "," Sign: ",$integer_type_sign," Length: ",$bitfield_length," StartBitPos: ",$bitfield_starting_bit_position;
                    
                    # For dumping
                    $dump_unit_size                     =   "'$bitfield_length;32'";    # Fixing the boundary as 32
                    $dump_memberType                    =   "bitfield";
                    $dump_type                          =   "BYTE";
                    $dump_array_size                    =   1;
                    $dump_display_typeName              =   "uint32";
                    $dump_array_dimension               =   0;
                }
            }
            elsif($type_name eq "array_type")
            {
                $array_type                             =   $type_node;
                
                # Get the array type
                # If the array is of primitive type then node type won't have "tag" field
                # If "tag" field is present then its either a structure or a union
                
                $type_node                              =   $entries[$array_type]{elts};
                $type_name                              =   $entries[$type_node]{node_name};
                
                # Get the length of the array - This is got from "array_type" node
                # Note: (min,max) specify the bounds of the array
                #       Just read max and return max+1 as the array size
                $integer_type                           =   $entries[$array_type]{domn};
                $integer_cst                            =   $entries[$integer_type]{max};
                $integer_cst_low                        =   $entries[$integer_cst]{low};
                $array_length                           =   $integer_cst_low+1;
                
                # Integer type
                if($type_name eq "integer_type")
                {
                    $integer_type                       =   $type_node;
                    
                    # Find the name of the type
                    $type_decl                          =   $entries[$integer_type]{name};
                    $identifier_node                    =   $entries[$type_decl]{name};
                    $identifier_node_strg               =   $entries[$identifier_node]{strg};
                    $array_type_name                    =   $identifier_node_strg;
                    
                    # Get the size of the type
                    $integer_cst                        =   $entries[$integer_type]{size};
                    $integer_cst_low                    =   $entries[$integer_cst]{low};
                    $integer_type_size                  =   $integer_cst_low;
                    
                    # Get the sign of the type
                    $integer_type_sign                  =   $entries[$integer_type]{sign};
                    
                    print testFile "Array of Type: ",$array_type_name," Size: ",$integer_type_size," Sign: ",$integer_type_sign," Length:",$array_length;
                    
                    # For dumping
                    $dump_unit_size                     =   ($integer_type_size/8);
                    $dump_memberType                    =   "default";
                    $dump_type                          =   "BYTE";
                    $dump_display_typeName              =   $array_type_name;
                    $dump_array_dimension               =   1;
                }
                elsif($type_name eq "enumeral_type")
                {
                    $enumeral_type                      =   $type_node;
                    
                    # Find the name of the enumeration
                    $type_decl                          =   $entries[$enumeral_type]{name};
                    $identifier_node                    =   $entries[$type_decl]{name};
                    $identifier_node_strg               =   $entries[$identifier_node]{strg};
                    $enumeration_type_name              =   $identifier_node_strg;
                    
                    # Find the size of the enumerator
                    $integer_cst                        =   $entries[$enumeral_type]{size};
                    $integer_cst_low                    =   $entries[$integer_cst]{low};
                    $enumerator_size                    =   $integer_cst_low;
                    
                    print testFile "Array of Type: Enumeration Name: ",$enumeration_type_name," Length:",$array_length;
                    
                    # For Dumping
                    $dump_memberType                    =   "enum";
                    $dump_unit_size                     =   ($enumerator_size/8);
                    $dump_type                          =   "BYTE";
                    $dump_display_typeName              =   $enumeration_type_name;
                    $dump_array_dimension               =   1;
                }
                elsif($type_name eq "array_type")
                {
                    $array_type_2                       =   $type_node;
                    $type_node                          =   $entries[$array_type_2]{elts};
                    
                    # Find the name of the type
                    $type_decl                          =   $entries[$type_node]{name};
                    $identifier_node                    =   $entries[$type_decl]{name};
                    $identifier_node_strg               =   $entries[$identifier_node]{strg};
                    $array_name                         =   $identifier_node_strg;
                    
                    # Find the array type
                    $array_type                         =   $entries[$type_node]{node_name};
                    
                    # Get the size of the type
                    $integer_cst                        =   $entries[$type_node]{size};
                    $integer_cst_low                    =   $entries[$integer_cst]{low};
                    $integer_type_size                  =   $integer_cst_low;
                    
                    # Get the sign of the type
                    $integer_type_sign                  =   $entries[$type_node]{sign};
                    
                    # Get the length of the array - This is got from "array_type" node
                    # Note: (min,max) specify the bounds of the array
                    #       Just read max and return max+1 as the array size
                    $integer_type                       =   $entries[$array_type_2]{domn};
                    $integer_cst                        =   $entries[$integer_type]{max};
                    $integer_cst_low                    =   $entries[$integer_cst]{low};
                    $array_length_2                     =   $integer_cst_low+1;
                    
                    print testFile "2-D Array of Type: ",$array_name," Size: ",$integer_type_size," Sign: ",$integer_type_sign," Length: ",$array_length,"x",$array_length_2;
                    
                    if($array_type eq "record_type")
                    {
                        # Structure Type
                        $dump_memberType                    =   "struct";
                        $dump_type                          =   "SUBREC";
                        $dump_unit_size                     =   $array_name;
                    }
                    elsif($array_type eq "union_type")
                    {
                        # Union Type
                        $dump_memberType                    =   "union";
                        $dump_type                          =   "SUBREC";
                        $dump_unit_size                     =   $array_name;
                    }
                    else
                    {
                        # Primitive Type
                        $dump_memberType                    =   "default";
                        $dump_type                          =   "BYTE";
                        $dump_unit_size                     =   ($integer_type_size/8);
                    }
                    
                    # New change don't convert 2-D array into 1-D array as we need this information for code generation
                    ## Convert length of 2-D array to 1-D array for simplicity
                    #$array_length                       *=  $array_length_2;
                    
                    # array_length for 2-D arrays will be of the form "x,y" where is x is the inner dimension and y is the outer dimension
                    $array_length                           =   $array_length.",".$array_length_2;
                    
                    # For dumping
                    $dump_display_typeName                  =   $array_name;
                    $dump_array_dimension                   =   2;
                }
                elsif($type_name eq "record_type")
                {
                    # Structures
                    
                    # Get the type
                    $type_node                          =   $entries[$array_type]{elts};
                    $type_name                          =   $entries[$type_node]{tag};
                    
                    # Get the type of the name
                    $type_decl                          =   $entries[$type_node]{name};
                    $identifier_node                    =   $entries[$type_decl]{name};
                    $identifier_node_strg               =   $entries[$identifier_node]{strg};
                    $array_name                         =   $identifier_node_strg;
                    
                    # Get the size of the structure
                    $integer_cst                        =   $entries[$type_node]{size};
                    $integer_cst_low                    =   $entries[$integer_cst]{low};
                    $struct_size                        =   $integer_cst_low;
                    
                    print testFile "Array of Type: ",$type_name," Name: ",$array_name," Length:",$array_length;
                    
                    # For dumping
                    # For sub records, type         = SUBREC
                    #                  unit_size    = Name of the sub record
                    $dump_type                          =   "SUBREC";
                    $dump_unit_size                     =   $array_name;
                    $dump_memberType                    =   "struct";
                    $dump_display_typeName              =   $array_name;
                    $dump_array_dimension               =   1;
                }
                elsif($type_name eq "union_type")
                {
                    # Union
                    
                    # Get the type
                    $type_node                          =   $entries[$array_type]{elts};
                    $type_name                          =   $entries[$type_node]{tag};
                    
                    # Get the type of the name
                    $type_decl                          =   $entries[$type_node]{name};
                    $identifier_node                    =   $entries[$type_decl]{name};
                    $identifier_node_strg               =   $entries[$identifier_node]{strg};
                    $array_name                         =   $identifier_node_strg;
                    
                    # Get the size of the union
                    $integer_cst                        =   $entries[$type_node]{size};
                    $integer_cst_low                    =   $entries[$integer_cst]{low};
                    $union_size                         =   $integer_cst_low;
                    
                    print testFile "Array of Type: ",$type_name," Name: ",$array_name," Length:",$array_length;
                    
                    # For dumping
                    # For sub records, type         = SUBREC
                    #                  unit_size    = Name of the sub record
                    $dump_type                          =   "SUBREC";
                    $dump_unit_size                     =   $array_name;
                    $dump_memberType                    =   "union";
                    $dump_display_typeName              =   $array_name;
                    $dump_array_dimension               =   1;
                }
                
                # For dumping
                $dump_array_size                        =   $array_length;
            }
            elsif($type_name eq "pointer_type")
            {
                $pointer_type                           =   $type_node;
                
                # Find to which type is the pointer pointing
                $type_node                              =   $entries[$pointer_type]{ptd};
                $type_name                              =   $entries[$type_node]{node_name};
                
                if($type_name eq "void_type")
                {
                    print testFile "Pointer to Type: void";
                    
                    # For dumping
                    $pointing_to_name                   =   "void";
                }
                elsif($type_name eq "function_type")
                {
                    # Get the function name to which the pointer is pointing to
                    $type_decl                          =   $entries[$pointer_type]{name};
                    $identifier_node                    =   $entries[$type_decl]{name};
                    $identifier_node_strg               =   $entries[$identifier_node]{strg};
                    $function_name                      =   $identifier_node_strg;
                    
                    print testFile "Pointer to function Name: ",$function_name;
                    
                    $pointing_to_name                   =   "function";
                }
                else
                {
                    # Pointing to primitive type or struct or union
                    $type_decl                          =   $entries[$type_node]{name};
                    $identifier_node                    =   $entries[$type_decl]{name};
                    $identifier_node_strg               =   $entries[$identifier_node]{strg};
                    $pointing_to_name                   =   $identifier_node_strg;
                    
                    print testFile "Pointer to Type:",$pointing_to_name;
                }
                
                # For dumping
                $dump_unit_size                         =   4;  # Pointer needs 4 bytes
                $dump_memberType                        =   "pointer";
                $dump_array_size                        =   1;
                $dump_display_typeName                  =   $pointing_to_name;
                $dump_type                              =   "BYTE";
                $dump_array_dimension                   =   0;
            }
            elsif($type_name eq "enumeral_type")
            {
                $enumeral_type                          =   $type_node;
                
                # Find the name of the enumerator
                $type_decl                              =   $entries[$enumeral_type]{name};
                $identifier_node                        =   $entries[$type_decl]{name};
                $identifier_node_strg                   =   $entries[$identifier_node]{strg};
                $enumerator_name                        =   $identifier_node_strg;
                
                # Find the size of the enumerator
                $integer_cst                            =   $entries[$enumeral_type]{size};
                $integer_cst_low                        =   $entries[$integer_cst]{low};
                $enumerator_size                        =   $integer_cst_low;
                
                print testFile "Type: Enumeral Name: ",$enumerator_name;
                
                # For dumping
                $dump_unit_size                         =   ($enumerator_size/8);
                $dump_memberType                        =   "enum";
                $dump_type                              =   "BYTE";
                $dump_array_size                        =   1;
                $dump_display_typeName                  =   $enumerator_name;
                $dump_array_dimension                   =   0;
            }
            elsif($type_name eq "record_type")
            {
                # Structure within a structure
                $record_type                            =   $type_node;
                    
                # Find the name of the structure    
                $type_decl                              =   $entries[$record_type]{name};
                $identifier_node                        =   $entries[$type_decl]{name};
                $identifier_node_strg                   =   $entries[$identifier_node]{strg};
                $structure_name                         =   $identifier_node_strg;
                
                # Get the size of the struct
                $integer_cst                            =   $entries[$record_type]{size};
                $integer_cst_low                        =   $entries[$integer_cst]{low};
                $struct_size                            =   $integer_cst_low;
                
                print testFile "Type: Struct Name: ",$structure_name;
                
                # For dumping
                # In case of struct and union, unit_size will have the name of the sub record
                $dump_type                              =   "SUBREC";
                $dump_unit_size                         =   $structure_name;
                $dump_memberType                        =   "struct";
                $dump_array_size                        =   1;
                $dump_display_typeName                  =   $structure_name;
                $dump_array_dimension                   =   0;
            }
            elsif($type_name eq "union_type")
            {
                # Union within a structure
                $union_type                             =   $type_node;
                    
                # Find the name of the structure    
                $type_decl                              =   $entries[$union_type]{name};
                $identifier_node                        =   $entries[$type_decl]{name};
                $identifier_node_strg                   =   $entries[$identifier_node]{strg};
                $union_name                             =   $identifier_node_strg;
                
                # Get the size of the struct
                $integer_cst                            =   $entries[$union_type]{size};
                $integer_cst_low                        =   $entries[$integer_cst]{low};
                $union_size                            =   $integer_cst_low;
                
                print testFile "Type: Union Name: ",$union_name;
                
                # For dumping
                # In case of struct and union, unit_size will have the name of the sub record
                $dump_type                              =   "SUBREC";
                $dump_unit_size                         =   $union_name;
                $dump_memberType                        =   "union";
                $dump_array_size                        =   1;
                $dump_display_typeName                  =   $union_name;
                $dump_array_dimension                   =   0;
            }

            print testFile"\n";
            
            push(@elements,{name => $field_member_type_name, type => $dump_type, size => $dump_unit_size, array_length => $dump_array_size, memberType => $dump_memberType, displayTypeName => $dump_display_typeName, arrayDimension => $dump_array_dimension});
        }
        else
        {
            # Stop parsing
            $parse = 0;
        }
        
        # Get the next field link
        $field_decl = $entries[$field_decl]{chan};
        if ($debug_mode)
        {
            print $fd "\t\tchain:",$field_decl,"\n";
        }
    }
    
    # Now copy all the entries into struct_data
    $data_struct{$struct_union_name}            =   [@elements];
    $recordType{$struct_union_name}             =   $struct_union_type;    # This records information about the record type, struct or union
    
    #print "\n";
    print testFile "\n";
    
    if ($debug_mode)
    {   
        close($fd);
    }
}

#sub parseTree
#{
#    my $numOfEntries = shift;
#    my $ix;
#
#    # Parse the data structure and make sense out of it
#    for($ix=1 ; $ix<=$numOfEntries ; $ix++)
#    {
#        $node_name = $entries[$ix]{node_name};
#        
#        # Handle typedef's
#        if($node_name eq 'type_decl')
#        {
#            parse_typedef($ix);
#        }
#        elsif($node_name eq 'enumeral_type')
#        {
#            parse_enumerator($ix);
#        }
#        elsif($node_name eq 'record_type' || $node_name eq 'union_type')
#        {
#            parse_struct_union($ix);
#        }
#    }
#}

sub parseDumpFile
{
    my $fileName = shift;
    
    return storeEntries($fileName);
}

sub getEnumerators
{
    my $numOfEntries    =   shift;
    my $ix;
    my $node_name;

    # Parse the data structure and make sense out of it
    for($ix=1 ; $ix<=$numOfEntries ; $ix++)
    {
        $node_name = $entries[$ix]{node_name};
        
        if($node_name eq 'enumeral_type')
        {
            parse_enumerator($ix);
        }
    }
    
    return \%enumHashList;
}

sub getRecords
{
    my $numOfEntries    =   shift;
    my $ix;
    my $node_name;
    
    open(testFile, ">output.txt") or die "$!";

    # Parse the data structure and make sense out of it
    for($ix=1 ; $ix<=$numOfEntries ; $ix++)
    {
        $node_name = $entries[$ix]{node_name};
        
        if($node_name eq 'record_type' || $node_name eq 'union_type')
        {
            parse_struct_union($ix);
        }
    }
    
    close(testFile);
    
    # Think about this
    # Before returning the reference to the structure, cleanup names
    # Remove any names starting with .... etc
    #foreach my $key(keys %data_struct)
    #{
    #    if($key =~ /\./)
    #    {
    #        delete $data_struct{$key};
    #    }
    #}
    
    return \%data_struct; # Return array reference
}

# This function returns the hash list of all records which has information about its type, struct/union
# This has to be called after the function parse_struct_union() as the list is built there.
sub getRecordTypeList
{
    return \%recordType;
}

1;

