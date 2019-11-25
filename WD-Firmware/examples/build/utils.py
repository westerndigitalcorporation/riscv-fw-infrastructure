import os
import platform

# this function creates a header string
def fnHeaderCreate(listHeader):
   strHeader = '\n _______________________________________\n'
   for strLine in listHeader:
      strHeader += '\n\t' + strLine
   strHeader += '\n _______________________________________\n'
   return strHeader
   
 # add 'size' util output to the map file
def fnProduceSectionsSize(target, source, env):
   strHeaderText = 'Sections size summary'
   # check if there was any change in the .map file
   if not strHeaderText in open(env['MAP_FILE']).read():
      # .map was updated so add the sections size
      strElfName = str(source[0])
      strSizeUtilName = os.path.join(env['TOOLS_BASE_DIR'], 'riscv64-unknown-elf-size')
      strHeader = fnHeaderCreate([strHeaderText])
      os.system('echo "' + strHeader + '" >> ' + env['MAP_FILE'])
      os.system(strSizeUtilName + ' ' + strElfName + ' >> ' + env['MAP_FILE']) 
   return None

# under linux, verify installation dependencies
def fnCheckInstalledDependencis(listDependencis):
  if platform.uname()[0] == 'Linux':
    for strDependency in listDependencis:
      os.system('dpkg -s '+ strDependency +' > tmp.txt 2>&1')
      if 'not installed' in open('tmp.txt').read():
        print("please install missing library - " + strDependency)
        os.system('rm tmp.txt')
        exit(1)
    os.system('rm tmp.txt')
  else: 
    # currently only linux is supported 
    print("unsupported environment, please switch to a linux based machine")
    exit(1)   
    
def set_outputfiles_names(name, prefix = "", suffix = ""):
    # format the artifacts files name for the current demo
    if prefix:
      name = "%s-%s" % (prefix, name)
    if suffix:
      name = "%s-%s" % (name, suffix)
    map_file = name + ".map"
    elf_file = name + ".elf"
    return map_file, elf_file
    
def fnCreateFolder(strPath):
  if not os.path.exists(strPath):
    os.makedirs(strPath)
  