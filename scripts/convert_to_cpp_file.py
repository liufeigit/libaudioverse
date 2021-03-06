import sys
import jinja2
import os
import os.path

if __name__ == '__main__':
    if len(sys.argv) != 5:
        print("Syntax: python convert_to_cpp_file.py <namespace> <array_name> <file_name> <outputfile>")
        sys.exit(1)

    namespace_name = sys.argv[1]
    array_name = sys.argv[2]
    input_file_name = sys.argv[3]
    output_file_name = sys.argv[4]

    template_string = """/**Copyright (C) Austin Hicks, 2014
This file is part of Libaudioverse, a library for 3D and environmental audio simulation, and is released under the terms of the Gnu General Public License Version 3 or (at your option) any later version.
A copy of the GPL, as well as other important copyright and licensing information, may be found in the file 'LICENSE' in the root of the Libaudioverse repository.  Should this file be missing or unavailable to you, see <http://www.gnu.org/licenses/>.*/
/**Note: this file was generated by a script using {{input_file_name}} and represents a blob of binary data.
No further contents is to be found in this file.
Do not edit.*/
#include <stdint.h>
namespace {{namespace_name}} {{"{"}}

char {{array_name}}[] = {{"{"}}{%for i in chars%}'{{i}}',{%if loop.index%40 == 0%}
{%endif%}{%endfor%}{{"}"}};
unsigned int {{array_name}}_size = {{chars|length}}*sizeof(char);
{{"}"}}
"""

    with open(input_file_name, "rb") as f:
        input_data = f.read()

    chars = []
    for i, j in enumerate(input_data):
        escape_string = "\\"+hex(j)[1:]
        chars.append(escape_string)


    env = jinja2.Environment()
    template = env.from_string(template_string)
    result = template.render(globals())

    #ensure directory exists:
    needed_directory = os.path.split(output_file_name)[0]
    if not os.path.exists(needed_directory):
        os.makedirs(needed_directory)


    with open(output_file_name, "wb") as f:
        f.write(result.encode('utf8'))
    print("Wrote", output_file_name)
