

class demo(object):
  def __init__(self):
    self.strName = "ex_freertos"
    self.rtos_core = "freertos"
    
    self.rtos_defines = ['D_USE_FREERTOS']
    
    self.public_defs = [
        'D_USE_RTOSAL',
        'D_MTIME_ADDRESS=0x0200BFF8',
        'D_MTIMECMP_ADDRESS=0x02004000',
        'D_CLOCK_RATE=32768',
        'D_TICK_TIME_MS=4',
        'D_ISR_STACK_SIZE=400',
        'D_USE_FREERTOS'
    ]
   
    self.listSconscripts = [
      'freertos',
      'psp',
      'rtosal',
      'demo_rtosal'
    ]
  
