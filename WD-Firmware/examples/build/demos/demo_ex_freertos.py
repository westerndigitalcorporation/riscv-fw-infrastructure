from demo_abstract import clsdemo_abstract

class demo(clsdemo_abstract):
  def __init__(self):
    clsdemo_abstract.__init__(self)
    self.name = "ex_freertos"
    self.rtos_core = "freertos"
    
    self.public_defs = [
        'D_USE_RTOSAL',
        'D_MTIME_ADDRESS=0x0200BFF8',
        'D_MTIMECMP_ADDRESS=0x02004000',
        'D_CLOCK_RATE=32768',
        'D_TICK_TIME_MS=4',
        'D_ISR_STACK_SIZE_WORDS=400'
    ]
    
    self.moduls = [
      'freertos',
      'rtosal',
      'psp'
    ]
  
