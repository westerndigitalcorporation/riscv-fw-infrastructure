

class clsdemo_abstract(object):
  def __init__(self):
    self.name = ""
    self.rtos_core = ""
    self.file_name = ""
    self.map_file = ""
    self.elf_file = ""
    self.public_defs = []
    self.moduls = []
    
  def set_outputfiles_names(self, prefix = "", suffix = ""):
    self.file_name = self.name
    if prefix:
      self.file_name = "%s-%s" % (prefix, self.name)
    if suffix:
      self.file_name = "%s-%s" % (self.file_name, suffix)
    self.map_file = self.file_name + ".map"
    self.elf_file = self.file_name + ".elf"
    
