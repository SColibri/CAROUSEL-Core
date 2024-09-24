# ----------------------
# Data models
# -----------------------------------------------------------------------------
# Developer: Sebastian Carrion
# Year: 2024
# 
# Brief: Creates python bindings for specific #python tags.
#
# Available Tags:
# - #pythonMethod ; Exposes method to python (Note: for now only allows one parameter)
# - #pythonProperty ; Exposes the get/set methods as a class property
# -----------------------------------------------------------------------------

# Imports
import re
from shlex import join
import sys
import os

# -----------------------------------------------------------------------------
#                                   HELPERS
# -----------------------------------------------------------------------------

# Define a function to convert a match to a dictionary
def match_to_dict(match):
    return {
        'summary': match[0],
        'additional_info_01': match[1],
        'parameters': match[2],
        'parameter_info': match[3],
        'parameter_description': match[4],
        'tag_section': match[5],
        'tag': match[6],
        'return_type': match[7],
        'method_name': match[8],
        'parameter_type': match[9],
        'parameter_name': match[10],
        'return_python_type' : get_python_type(match[7]),
        'parameter_python_type': get_python_type(match[9])
    }

# Packs all required data for generating the embedded python files into one dictionary
def pack_data(filename, outputDirectory):
    with open(filename, 'r') as file:
        content = file.read()
        
    [mappedResults, mappedMethods, mappedProperties] = get_mapped_items(content)
    classNamespace = get_namespace(content)

    return {
        'mappedResults' : mappedResults,
        'mappedMethods' : mappedMethods, 
        'mappedProperties' : mappedProperties, 
        'className' : get_class_name(content), 
        'classDescription' : get_class_description(content), 
        'outputFile' : outputDirectory + '/' + namespace_to_camel_case(classNamespace) + 'PythonBindings' , 
        'headerInclude' : get_relative_path(filename, outputDirectory),
        'namespace' : classNamespace
    }
    
# Returns the direct 
def get_python_type(parameterName):
    
    if 'std::vector<std::string>' in parameterName:
        parameterType = 'List[str]'
    elif 'std::vector<int>' in parameterName:
        parameterType = 'List[int]'
    elif 'std::vector<float>' in parameterName:
        parameterType = 'List[float]'
    elif 'std::string' in parameterName or 'string' in parameterName:
        parameterType = 'str'
    elif 'int' in parameterName:
        parameterType = 'int'
    elif 'float' in parameterName:
        parameterType = 'float'
    elif 'double' in parameterName:
        parameterType = 'float'  # In Python, float covers double
    elif 'bool' in parameterName:
        parameterType = 'bool'
    elif 'char' in parameterName:
        parameterType = 'str'  # Single character in Python
    else:
        parameterType = parameterName  # Keep original if not recognized
        
    return parameterType

# Returns the import required for a specific python type
def get_import(pythonType):
    
    requiredImport = ''

    if 'List' in pythonType:
        requiredImport = 'from typing import List'
    elif 'Dict' in pythonType:
        requiredImport = 'from typing import Dict'
    elif 'Tuple' in pythonType:
        requiredImport = 'from typing import Tuple'
    elif 'Optional' in pythonType:
        requiredImport = 'from typing import Optional'
    elif 'Union' in pythonType:
        requiredImport = 'from typing import Union'
    elif 'Any' in pythonType:
        requiredImport = 'from typing import Any'
    elif 'Set' in pythonType:
        requiredImport = 'from typing import Set'
    elif 'Iterable' in pythonType:
        requiredImport = 'from typing import Iterable'

    return requiredImport

# returns the class name
def get_class_name(content):
    classNameMatch = re.findall(r'class\s(\w+).*\s+{', content)
    return classNameMatch[0]

# returns the namespace for a class
def get_namespace(content):
    classNamespaceMatch = re.findall(r'namespace\s(\w+)\s+{', content)
    return '::'.join(classNamespaceMatch)

# Returns the class description
def get_class_description(content):
    classDescription = re.findall(r'<summary>\s*([\s\S]*?)\s*\/\/\/\s*<\/summary>\s*class', content)
    return classDescription[0]

# returns the relative path from output directory to header file
def get_relative_path(filename, outputDirectory):
    relativePath = os.path.relpath(filename, outputDirectory)
    return str(relativePath)

# Converts namespace into camel case
def namespace_to_camel_case(input_str):
    parts = input_str.split('::')
    camel_case = parts[0].lower() + ''.join(part.capitalize() for part in parts[1:])
    return camel_case

# Returns a list of all required imports
def get_list_of_imports(mappedList):
    importList = set() 
    
    # Iterate over mappedList and add imports only if they are required
    for item in mappedList:
        for key in ['return_python_type', 'parameter_python_type']:
            pythonType = item.get(key)
            if pythonType: 
                requiredImport = get_import(pythonType)
                if requiredImport:  
                    importList.add(requiredImport)
    
    return list(importList)

# returns mapped properties and methods
def get_mapped_items(content):
    # Find all tags
    allMethods = re.findall(r'\/\/\/\s*<summary>\s*([\s\S]*?)\s*\/\/\/\s<\/summary>\s+(\/\/\/\s<returns>(.*?)<\/returns>\s+)?(\/\/\/\s<param(.*?)<\/param>\s+)?(\/\/\/\s<tag(.*?)<\/tag>\s+)?(.*?)(\w+)\((.*?)(\w+)?\)', content)

    # Map al matches into a dictionary
    mappedResults = [match_to_dict(match) for match in allMethods]
    mappedMethods = [result for result in mappedResults if '#pythonMethod' in result['tag']]
    mappedProperties = [result for result in mappedResults if '#pythonProperty' in result['tag']]
    
    return [mappedResults, mappedMethods, mappedProperties]

# returns the pybind11 class deifintion that exposes a c++ class to python
def get_pybind_class_definition(file, data):
    file.write(f"\t\t    pybind11::class_<{data['namespace']}::{data['className']}>(carouselModule, \"{data['className']}\")\n")

    # Add default constructor
    file.write('\t\t        .def(pybind11::init<>())\n')

    # Add properties 
    for mappedProperty in data['mappedProperties']:
        methodName = mappedProperty['method_name']
        parameterName = mappedProperty['parameter_name']
        
        # Setter method should have one parameter
        if not parameterName:
            raise Exception(f'Property tag #pythonProperty should be set on a set method with one parameter: Method - {methodName} || {data["namespace"]}::{data["className"]}')

        lineToAdd = f"\t\t        .def_property(\"{methodName.replace('set','')}\", &{data['namespace']}::{data['className']}::{methodName}, &{data['namespace']}::{data['className']}::{methodName.replace('set', 'get')})\n"
        file.write(lineToAdd)

    # Add method matches (TODO: do we need more than one parameter?)
    for mappedMethod in data['mappedMethods']:
        methodName = mappedMethod['method_name']
        parameterName = mappedMethod['parameter_name']
        
        if parameterName:
            lineToAdd = f"\t\t        .def(\"{methodName}\", &{data['namespace']}::{data['className']}::{methodName}, pybind11::arg(\"{parameterName}\"))\n"
        else:
            lineToAdd = f"\t\t        .def(\"{methodName}\", &{data['namespace']}::{data['className']}::{methodName})\n"
        file.write(lineToAdd)
        print(f'{lineToAdd}')
        
    #eof
    file.write('\t\t        ;\n')

# returns the python stubbed class for IDE intellisense
def get_python_class_definition(file, data):
     # Create class structure
    file.write('\n\n')
    file.write(f'class {data["className"]}:\n')
    
    # Add class summary to stub file
    if data['classDescription']:
        file.write(f'\t"""\n\t{data["classDescription"].replace("///","")}\n\t"""\n\n')
        
    # Add properties
    for mappedProperty in data['mappedProperties']:
        methodDescription = mappedProperty['summary']
        methodName = mappedProperty['method_name'].replace('set','')
        file.write(f'\t{methodName} : str = ...\n')
        file.write(f'\t"""\n\t{methodDescription.replace("///","")}\n\t"""\n\n')
    
    # Add Methods (TODO: do we need more than one parameter?)
    for mappedMethod in data['mappedMethods']:
        methodName = mappedMethod['method_name']
        parameterName = mappedMethod['parameter_name']
        methodDescription = mappedMethod['summary']
        
        if not parameterName:
            file.write(f'\tdef {methodName}(self): ...\n')
        else:
            parameterType = get_python_type(mappedMethod['parameter_type']);

            file.write(f'\tdef {methodName}(self, {parameterName} : {parameterType}): ... \n')
        
        file.write(f'\t"""\n\t{methodDescription.replace("///","")}\n\t"""\n\n')

# -----------------------------------------------------------------------------
#                                   PYTHON BINDINGS
# -----------------------------------------------------------------------------

# Map input
outputDirectory = sys.argv[1]        
moduleName = sys.argv[2]
headerFile = sys.argv[3:]
outputFile = outputDirectory + '/' + moduleName + 'PythonBindings.h' 

# read all files and pack details
toEmbeddData = [pack_data(filename, outputDirectory) for filename in headerFile]
    
# Create binding file
with open(outputFile, 'w') as file:
    
    # Create file and add includes
    file.write('#pragma once\n')
    file.write('#include <pybind11/pybind11.h>\n')
    
    # Add all include directives required
    for data in toEmbeddData:
        file.write(f'#include \"{data["headerInclude"]}\"\n\n')
    
    # Namespace carousel :: scripting
    file.write('namespace carousel \n {\n')
    file.write('\tnamespace scripting \n\t{\n')
    
    # Create static method used for exposing the methods
    file.write('\t\tstatic void expose_models(pybind11::module& carouselModule) {\n\n')
    
    # Add all properties
    for data in toEmbeddData:
        file.write(f'\n\t\t\t// {data["className"]} - python embedding \n')
        get_pybind_class_definition(file, data)
        
    file.write('\t\t}\n\t}\n}')
    
# Create stub file
with open(outputFile.replace(".h",".pyi"), 'w') as file:
    # Add Imports
    for data in toEmbeddData:
        importList = get_list_of_imports(data['mappedResults'])
        for importItem in importList:
            file.write(f'{importItem}\n')

    # Create class structure
    for data in toEmbeddData:        
        get_python_class_definition(file, data)