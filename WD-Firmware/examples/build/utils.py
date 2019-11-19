import os
import platform

# this function creates a header string
def header_create(str_arr):
   header = '\n _______________________________________\n'
   for line in str_arr:
      header += '\n\t' + line
   header += '\n _______________________________________\n'
   return header
   
 # add 'size' util output to the map file
def produce_sections_size(target, source, env):
   text = 'Sections size summary'
   # check if there was any change in the .map file
   if not text in open(env['MAP_FILE']).read():
      # .map was updated so add the sections size
      elf_name = str(source[0])
      size_util_name = os.path.join(env['TOOLS_BASE_DIR'], 'riscv64-unknown-elf-size')
      header = header_create([text])
      os.system('echo "' + header + '" >> ' + env['MAP_FILE'])
      os.system(size_util_name + ' ' + elf_name + ' >> ' + env['MAP_FILE']) 
   return None

# under linux, verify installation dependencies
def check_installed_dependencis(dependency_list):
  if platform.uname()[0] == 'Linux':
    for dependency in dependency_list:
      os.system('dpkg -s '+ dependency +' > tmp.txt 2>&1')
      if 'not installed' in open('tmp.txt').read():
        print("please install missing library - " + dependency)
        os.system('rm tmp.txt')
        exit(1)
    os.system('rm tmp.txt')
  else: 
    # currently only linux is supported 
    print("unsupported environment, please switch to a linux based machine")
    exit(1)   
 
def createFolder(strPath):
  if not os.path.exists(strPath):
    os.makedirs(strPath)