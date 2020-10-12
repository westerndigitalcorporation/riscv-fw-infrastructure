import gdb
import re
import sys

from gdb.unwinder import Unwinder
from gdb.FrameDecorator import FrameDecorator

# Required to make calls to super () work in python2.
__metaclass__ = type

#=====================================================================#

# Notes on ComRV stack management.
#
# The ComRV stack is full-descending.
#
# Register $t3 points to the current comrv stack frame, that is the
# frame that was used for the last call into ComRV.
#
# Register $t4 points to the next comrv stack frame to use, this is the
# frame that will be used by the next call into ComRV.
#
# Each ComRV stack frame can potentially be of different sizes, though
# at the time of writing this, I have never seen a non-standard sized
# ComRV stack frame in the wild.  It might be the case that only when
# using an RTOS will ComRV stack frames be anything other than the
# standard size(s).
#
# The layout and size of a standard ComRV stack frame depends on
# whether the application has multi-group support compiled in, and
# then there are two versions of multi-group support, depending on how
# large your multi-groups might be.
#
# This is one layout of a stack frame (size 12-bytes):
#
# |0|1|2|3|	[ Byte numbering, for information, not part of the stack. ]
# |  A|B|C|	A=Offset (2-bytes), B=Alignment (1-byte), C=Multi-group Index (1-byte)
# |       |	4-byte token.
# |       |	4-byte return address.
#
# This is another layout of a stack frame (size 16-bytes):
#
# |0|1|2|3|	[ Byte numbering, for information, not part of the stack. ]
# |  D|  E|	D=Padding (2-bytes), E=Multi-group Index (2-bytes)
# |  A|B|C|	A=Offset (2-bytes), B=Alignment (1-byte), C=Padding (1-byte)
# |       |	4-byte token.
# |       |	4-byte return address.
#
# The first stack frame entry, created when the comrv enginee is
# initialised will have return address, token, and alignment fields
# all set to 0.  The Offset field will be set to 0xdead.  This entry
# marks the outer most entry of the comrv stack, we never unwind past
# this.  To try to do so is a mistake.
#
# The 'Offset' field: This contains the offset in bytes between the
# current comrv stack frame pointer and the previous comrv-stack frame
# pointer.
#
# The 'Alignment' is used during the process of unwinding.  When we
# have a return address within the cache, and we know which overlay
# group used to be mapped in, the alignment is used as part of the
# process of figuring out the correct return address.
#
# The 'Token' field contains the token that was requested when ComRV
# was called into.  This will represent the overlay that is going to
# be loaded into memory with this call (or maybe the overlay will
# already be loaded).
#
# When calling a non-overlay function from an overlay function we
# still need to pass through ComRV, so returns from the non-overlay
# function will, upon passing through ComRV trigger the overlay
# function to be mapped back in.  As a result, the 'Token' field can
# also contain real addresses in non-overlay functions.
#
# The 'return address' field is the value of $ra on entry into ComRV.
# If we call to ComRV from non-overlay code, then this will be a real
# address we can return too.  If we call into ComRV from overlay code
# then this address will be in the cache and we will need to figure
# out which overlay used to be mapped, and recompute a new return
# address.




#=====================================================================#

# Should debug messages be printed?
DEFAULT_DEBUG = False

# The default maximum group size.
DEFAULT_MAX_GROUP_SIZE = 4096

# The default size for the "pages" in the ComRV cache and storage area.
DEFAULT_MIN_COMRV_CACHE_ENTRY_SIZE_IN_BYTES = 512
OVERLAY_MIN_CACHE_ENTRY_SIZE_IN_BYTES = 512
COMRV_INFO_EVICTION_FIELD = 0x30
COMRV_INFO_EVICT_POLICY_LRU = 0x10
COMRV_INFO_EVICT_POLICY_LFU = 0x20
COMRV_INFO_EVICT_POLICY_MIX = 0x30

# Various symbols that are read in order to parse ComRV.
MULTI_GROUP_OFFSET_SYMBOL = "g_stComrvCB.ucMultiGroupOffset"
COMRV_INFO_SYMBOL = "g_uiComrvInfo"
OVERLAY_STORAGE_START_SYMBOL = "OVERLAY_START_OF_OVERLAYS"
OVERLAY_STORAGE_END_SYMBOL = "OVERLAY_END_OF_OVERLAYS"
OVERLAY_CACHE_START_SYMBOL = "__OVERLAY_CACHE_START__"
OVERLAY_CACHE_END_SYMBOL = "__OVERLAY_CACHE_END__"
COMRV_RETURN_FROM_CALLEE_LABEL = "comrv_ret_from_callee"
COMRV_RETURN_FROM_CALLEE_CONTEXT_SWITCH_LABEL = "comrv_ret_from_callee_context_switch"
COMRV_IGONR_CALLER_THUNK_STACK_FRAME = "comrv_igonr_caller_thunk_stack_frame"
COMRV_INVOKE_CALLEE_LABEL = "comrv_invoke_callee"
COMRV_ENTRY_LABEL = "comrvEntry"
COMRV_END_LABEL = "comrvEntryDisable"
COMRV_ENTRY_CONTEXT_SWITCH_LABEL = "comrvEntry_context_switch"
COMRV_EXIT_LABEL = "comrv_exit_ret_to_caller"

# The following symbols are actually used as format strings.  They must
# include a single '%d' format specified which is replaced with the ComRV
# cache index.
OVERLAY_CACHE_AT_INDEX_TO_GROUP_ID \
    = "g_stComrvCB.stOverlayCache[%d].unToken.stFields.uiOverlayGroupID"
OVERLAY_CACHE_AT_INDEX_TO_SIZE_IN_MIN_UNITS \
    = "g_stComrvCB.stOverlayCache[%d].unProperties.stFields.ucSizeInMinGroupSizeUnits"
OVERLAY_TABLE_ENTRY_EVICT_LOCK_VAL \
    = "g_stComrvCB.stOverlayCache[%d].unProperties.stFields.ucEvictLock"
OVERLAY_TABLE_ENTRY_GROUP_LOCK_VAL \
    = "g_stComrvCB.stOverlayCache[%d].unProperties.stFields.ucEntryLock"
OVERLAY_TABLE_ENTRY_DATA_OVL_VAL \
    = "g_stComrvCB.stOverlayCache[%d].unProperties.stFields.ucData"
OVERLAY_TABLE_ENTRY_LRU_EVICTION_VAL \
    = "g_stComrvCB.stOverlayCache[%d].unLru.stFields.typNextLruIndex"
OVERLAY_TABLE_ENTRY_LRU_INDEX_VAL \
    = "g_stComrvCB.ucLruIndex"
OVERLAY_TABLE_ENTRY_MRU_INDEX_VAL \
    = "g_stComrvCB.ucMruIndex"
OVERLAY_TABLE_ENTRY_TOKEN_VAL \
    = "g_stComrvCB.stOverlayCache[%d].unToken.uiValue"

#=====================================================================#

# A class for the control variable 'set/show debug comrv on|off'.
class debug_parameter (gdb.Parameter):
    '''Controls debugging messages from the Python Overlay Manager.  This
should only be needed to help track down bugs in the Python code.'''
    set_doc = "Set whether debugging from the Python Overlay Manager is on."
    show_doc = "Show whether debugging from the Python Overlay Manager is on."
    def __init__ (self):
        gdb.Parameter.__init__ (self, "debug comrv",
                                gdb.COMMAND_MAINTENANCE,
                                gdb.PARAM_BOOLEAN)
        self.value = DEFAULT_DEBUG

    def get_show_string (self, value):
        return ("Debugging of ComRV Python overlay manager is %s."
                % (value))

    def __nonzero__ (self):
        if (self.value):
            return 1
        else:
            return 0

    def __bool__ (self):
        return self.value

# An instance of the debug parameter.  Due to operator overloading
# this can be treated as a boolean like:
#   if (overlay_debug):
#     ...
overlay_debug = debug_parameter ()

# Class to create the 'set comrv' prefix command.
class set_comrv_prefix_command (gdb.Command):
    def __init__ (self):
        gdb.Command.__init__ (self, "set comrv", gdb.COMMAND_NONE, gdb.COMPLETE_NONE, True)

# Class to create the 'show comrv' prefix command.
class show_comrv_prefix_command (gdb.Command):
    def __init__ (self):
        gdb.Command.__init__ (self, "show comrv", gdb.COMMAND_NONE, gdb.COMPLETE_NONE, True)

# Now instantiate the above classes, actually creating the 'set comrv' and
# 'show comrv' prefix commands.
set_comrv_prefix_command ()
show_comrv_prefix_command ()

# Class that represents the maximum overlay group size.  This is used when
# unwinding the ComRV stack.
class max_group_size_parameter (gdb.Parameter):
    '''The maximum group size.  This is a hard coded constant within the
toolchain and ComRV.  The value here must be adjusted to match.  This value
is required in order to correctly unwind the ComRV stack.

Changing this once GDB has already parsed the ComRV data structures will
cause undefined behaviour.  This should only be modified once, immediately
after initially loading the ComRV support Pythong script.'''
    set_doc = "Set the maximum overlay group size."
    show_doc = "Show the maximum overlay group size."
    def __init__ (self):
        gdb.Parameter.__init__ (self, "comrv max-group-size",
                                gdb.COMMAND_STACK,
                                gdb.PARAM_ZUINTEGER)
        self.value = DEFAULT_MAX_GROUP_SIZE

    def get_show_string (self, value):
        return ("Maximum ComRV overlay group size is %s." % (value))

# Instance of parameter object.  Use the value field of this object.
max_group_size = max_group_size_parameter ()

# Class that represents the size of the "pages" in the ComRV cache and
# storage area.  This is the minimum unit size in which overlays are
# measured.
class min_overlay_entry_size_parameter (gdb.Parameter):
    '''The minimum size for a ComRV overlay.  Many aspects of Comrv are
measured in multiples of this minimum size.  This value must match the
value used by the compiler, linker, and ComRV enginee.

Changing this once GDB has already parsed the ComRV data structures will
cause undefined behaviour.  This should only be modified once, immediately
after initially loading the ComRV support Pythong script.'''
    set_doc = "Set the minimum ComRV entry size."
    show_doc = "Show the minimum ComRV entry size."
    def __init__ (self):
        gdb.Parameter.__init__ (self, "comrv min-entry-size",
                                gdb.COMMAND_STACK,
                                gdb.PARAM_ZUINTEGER)
        self.value = DEFAULT_MIN_COMRV_CACHE_ENTRY_SIZE_IN_BYTES

    def get_show_string (self, value):
        return ("Minimum ComRV entry size, in bytes, is %s." % (value))

# Instance of parameter object.  Use the value field of this object.
min_entry_size = min_overlay_entry_size_parameter ()

# A class for the control variable 'set/show comrv show-frames'.
class show_comrv_frames_parameter (gdb.Parameter):
    '''Controls whether to show the comrv frames in the backtrace.  When
    this is off 'comrv' frames will be hidden unless they are the
    currently selected frame.'''
    set_doc = "Set whether ComRV frames are shown in the backtrace."
    show_doc = "Show whether ComRV frames are shown in the backtrace."
    def __init__ (self):
        gdb.Parameter.__init__ (self, "comrv show-frames",
                                gdb.COMMAND_STACK,
                                gdb.PARAM_BOOLEAN)
        self.value = True

    def get_show_string (self, value):
        return ("Display of ComRV frames in the backtrace is %s."
                % (value))

    def __nonzero__ (self):
        if (self.value):
            return 1
        else:
            return 0

    def __bool__ (self):
        return self.value

show_comrv_frames = show_comrv_frames_parameter ()

# A class for the control variable 'set/show comrv show-token'.
class show_comrv_tokens_parameter (gdb.Parameter):
    '''Controls whether to show the comrv token in the backtrace.  When
    this is on GDB will display the ComRV token passed to each 'comrv'
    frame.

    The tokens can only be displayed when 'comrv show-frames' is on.'''
    set_doc = "Set whether ComRV tokens are shown in the backtrace."
    show_doc = "Show whether ComRV tokens are shown in the backtrace."
    def __init__ (self):
        gdb.Parameter.__init__ (self, "comrv show-tokens",
                                gdb.COMMAND_STACK,
                                gdb.PARAM_BOOLEAN)
        self.value = True

    def get_show_string (self, value):
        return ("Display of ComRV tokens in the backtrace is %s."
                % (value))

    def __nonzero__ (self):
        if (self.value):
            return 1
        else:
            return 0

    def __bool__ (self):
        return self.value

show_comrv_tokens = show_comrv_tokens_parameter ()

# A class for the control variable 'set/show comrv initialized'.
class comrv_initialized_parameter (gdb.Parameter):
    '''This parameter displays whether the ComRV engine is currently
    initialized.  This parameter will initially be `off`, but will
    automatically enter the `on` state when GDB detects that ComRV
    must now be initialized.

    Manually switching this parameter back to `off` will cause GDB
    to discard the currently cached ComRV state.  However, the next
    time GDB detects that ComRV must be enabled this parameter will
    be switched back to `on`.

    Alternatively, manually switching this parameter to `on` will
    cause GDB to assume that ComRV is initialized.  GDB will parse
    the static ComRV and cache it.'''
    set_doc = "Set whether ComRV is currently initialized."
    show_doc = "Show whether ComRV is currently initialized."
    def __init__ (self):
        gdb.Parameter.__init__ (self, "comrv initialized",
                                gdb.COMMAND_STACK,
                                gdb.PARAM_BOOLEAN)
        self.value = False

    # Called to display this property.
    def get_show_string (self, value):
        if (value):
            return ("The ComRV engine is considered initialized.")
        else:
            return ("The ComRV engine is NOT considered initialized.")

    # Called to print a string when the user sets this property.  We
    # make use of this to tweak the state when this property is
    # adjusted.
    def get_set_string (self):
        if (not self.value):
            # User has requested that we cosider ComRV not
            # initialized.  Discard any existing cached data.
            overlay_data.clear ()
        return ""

    # Allow this property to be treated as an integer.
    def __nonzero__ (self):
        if (self.value):
            return 1
        else:
            return 0

    # Allow this property to be treated as a boolean.
    def __bool__ (self):
        return self.value

# An instance of this property.
is_comrv_initialized_p =  comrv_initialized_parameter ()

#=====================================================================#

# Return True if we believe that ComRV should have been initialised,
# and it is therefore safe to try and read the ComRV tables from
# memory.  Otherwise, return False.
def global_has_comrv_been_initialised_yet ():
    global is_comrv_initialized_p
    return is_comrv_initialized_p.value

# Mark the ComRV engine as initialised.
def global_mark_comrv_as_initialised ():
    global is_comrv_initialized_p
    is_comrv_initialized_p.value = True

#=====================================================================#

# Print STRING as a debug message if OVERLAY_DEBUG is True.
def debug (string):
    global overlay_debug

    if not overlay_debug:
        return

    print (string)
    sys.stdout.flush()

# Helper class, create an instance of this to temporarily turn on
# debug for the enclosing scope, and turn debug off when we leave the
# scope.
class temp_debug_on:
    def __init__ (self):
        global overlay_debug
        self._old_overlay_debug = overlay_debug
        overlay_debug = True

    def __del__ (self):
        global overlay_debug
        overlay_debug = self._old_overlay_debug

#=====================================================================#

# Thanks to: https://stackoverflow.com/a/32031543/3228495
def sign_extend (value, bits):
    sign_bit = 1 << (bits - 1)
    return (value & (sign_bit - 1)) - (value & sign_bit)

# Wrapper around access to the global configuration parameter.
def get_comrv_min_entry_size ():
    global min_entry_size
    return min_entry_size.value

# Get the address of LABEL which is a string.  If the address of LABEL
# can't be found then return None.
def get_symbol_address (label):
    try:
        return int (gdb.parse_and_eval ("&%s" % (label)))
    except:
        return None

# Return the gdb.Block of the function containing address ADDR, or
# None if the block could not be found.
def function_block_at (addr):
    block = gdb.current_progspace().block_for_pc(addr)
    if (block == None or block.global_block == None):
        return None
    while (not (block.superblock.is_global or block.superblock.is_static)):
        block = block.superblock
    return block

# Takes TOKEN_OR_ADDRESS which could be a 32-bit token, or a valid
# 32-bit RISC-V code address and returns true if it is a ComRV token,
# or false if it is a code address.
#
# This choice is based on the least-significant bit of the value in
# TOKEN_OR_ADDRESS, as a valid code address must have the least
# significant bit set to zero, while ComRV tokens have the least
# significant bit set to 1.
def is_overlay_token_p (token_or_address):
    return ((token_or_address & 0x1) == 1)

# Takes TOKEN which is a 32-bit multi-group token and returns true if
# TOKEN is a multi-group token, otherwise, returns false.
def is_multi_group_token_p (token):
    assert (is_overlay_token_p (token))
    return ((token >> 31) & 0x1) == 1

# Takes TOKEN which is a 32-bit multi-group token and returns the
# overlay group number extracted from the token.
def mg_token_group_id (token):
    assert (is_multi_group_token_p (token))
    return (token >> 1) & 0xffff

# Takes TOKEN, a non-multi-group token, and extract the group-id from
# the token.
def overlay_token_group_id (token):
    assert (not is_multi_group_token_p (token))
    return (token >> 1) & 0xffff

# Takes TOKEN, a non-multi-group token, and extract the function
# offset in bytes for the function referenced by this token.
def overlay_token_func_offset (token):
    assert (not is_multi_group_token_p (token))
    return ((token >> 17) & 0x3ff) * 4

# Class to wrap reading memory.  Provides an API for reading unsigned
# values of various sizes from memory.
class mem_reader:
    # Read a value LENGTH bytes long from ADDRESS.  The returned value
    # is unsigned.
    @staticmethod
    def _read_generic (address, length):
        inf = gdb.selected_inferior ()
        b = inf.read_memory (address, length)
        shift = 0
        val = 0
        for i in range(len(b)):
            t = ord (b[i])
            t <<= shift
            val |= t
            shift += 8

        return val

    @staticmethod
    def read_8_bit (address):
        return mem_reader._read_generic ((address & 0xffffffff), 1)

    @staticmethod
    def read_16_bit (address):
        return mem_reader._read_generic ((address & 0xffffffff), 2)

    @staticmethod
    def read_32_bit (address):
        return mem_reader._read_generic ((address & 0xffffffff), 4)

# The Overlay Cache Area is defined by a start and end label, this is
# the area into which code (and data?) is loaded in order to use it.
# This area is divided into "pages", each page is (currently) 512
# bytes (0x200) in size, but this can be modified by the user.
# The overlay tables are loaded into the last page of this cache
# area.
class overlay_data:
    _instance = None

    # Holds information about all the groups and multi-groups.
    class _overlay_group_data:
        def __init__ (self, groups, multi_groups, multi_group_table):
            self._groups = groups
            self._multi_groups = multi_groups
            self._multi_group_table = multi_group_table

        def get_group (self, index):
            return self._groups[index]

        def get_group_count (self):
            return len (self._groups)

        def get_multi_group (self, index):
            return self._multi_groups[index]

        def get_multi_group_count (self):
            return len (self._multi_groups)

        def get_token_from_multi_group_table (self, index):
            return self._multi_group_table[index]

    # Holds information about a single group.
    class _overlay_group:
        def __init__ (self, base_address, size_in_bytes, id):
            self._base_address = base_address
            self._size_in_bytes = size_in_bytes
            self._id = id

        def base_address (self):
            return self._base_address

        def size_in_bytes (self):
            return self._size_in_bytes

        @property
        def id (self):
            return self._id

    # Holds information about a single member of a multi-group.
    class _overlay_multi_group_member:
        def __init__ (self, overlay_group, token):
            self._overlay_group = overlay_group
            self._token = token
            self._offset = overlay_token_func_offset (token)

        @property
        def token (self):
            return self._token

        @property
        def overlay_group (self):
            return self._overlay_group

        @property
        def offset (self):
            return self._offset

    # Holds information about a single multi-group.  NUMBER is
    # assigned to each multi-group in the order they are encountered
    # in the multi-group table, with 0 assigned to the first
    # multi-group, then 1, etc.
    #
    # The INDEX is the index into the multi-group table for the first
    # token of that multi-group, so the first multi-group always has
    # index 0, but the second multi-group could have any index value.
    #
    # The MEMBERS is the list of multi-group member objects.
    #
    # The SIZE is the size in bytes of the function that is the
    # goal of this multi-group.
    #
    # The FUNC is a string, the name of the function this is a
    # multi-group for, or None if this couldn't be figured out.
    class _overlay_multi_group:
        def __init__ (self, number, index, members, size, func):
            self._number = number
            self._index = index
            self._members = members
            self._size_in_bytes = size
            self._function = func

        @property
        def tokens (self):
            return map (lambda m : m.token,
                        self._members)

        def index (self):
            return self._index

        def number (self):
            return self._number

        @property
        def members (self):
            return self._members

        @property
        def size_in_bytes (self):
            return self._size_in_bytes

        @property
        def function_name (self):
            return self._function

    # A class to describe an area of memory.  This serves as a base
    # class for the cache region descriptor, and the storage region
    # descriptor classes.
    class _memory_region:
        # The START is the first address within the region, while END
        # is the first address just beyond the region.
        def __init__ (self, start, end):
            self._start_address = start
            self._end_address = end

        # Returns the first address within the region.
        def start_address (self):
            return self._start_address

        # Return the first address past the end of the region.
        def end_address (self):
            return self._end_address

    # A static description of the overlay cache area.  This is the
    # area of memory into which overlays are loaded so they can be
    # used.
    class _cache_descriptor (_memory_region):
        def __init__ (self, start, end):
            super (overlay_data._cache_descriptor, self).__init__ (start, end)

        # Return the address for the start of the cache region.
        def base_address (self):
            return self.start_address ()

        # Return the total size of the cache in bytes, including the tables
        # region.
        def size_in_bytes (self):
            return self.end_address () - self.start_address ()

        # Return the number of entries that are available for holding
        # overlays.  This excludes the area that is given up to hold the
        # overlay tables.  Currently the tables are copied into the last entry
        # in the cache.
        def number_of_working_entries (self):
            entry_size = self.entry_size_in_bytes ()
            return ((self.size_in_bytes () / entry_size)
                    - (self.tables_size_in_bytes () / entry_size))

        # Return the total number of entries that are in the cache, this
        # includes any entries being used to hold the overlay tables.
        def total_number_of_entries (self):
            entry_size = self.entry_size_in_bytes ()
            return (self.cache_size_in_bytes () / entry_size)

        # The address of the overlay tables within the cache.  Currently these
        # are always in the last entry of the cache, and are one entry in size.
        def tables_base_address (self):
            entry_size = self.entry_size_in_bytes ()
            return self.end_address () - self.tables_size_in_bytes ()

        # Return the size of the overlay tables region in bytes.  This is
        # currently always a single page of the cache.
        def tables_size_in_bytes (self):
            return self.entry_size_in_bytes ()

        # Return the size in bytes of a single entry (or page) within the
        # cache.
        def entry_size_in_bytes (self):
            return get_comrv_min_entry_size ()

    # A class that describes the overlay systems storage area.  This
    # is the area of memory from which the overlays are loaded.  The
    # debug information will refer to this area,
    class _storage_descriptor (_memory_region):
        def __init__ (self, start, end):
            super (overlay_data._storage_descriptor, self).__init__ (start, end)

    class _comrv_labels ():
        def __init__ (self):
            self.comrv_invoke_callee \
                = get_symbol_address (COMRV_INVOKE_CALLEE_LABEL)
            self.ret_from_callee \
                = get_symbol_address (COMRV_RETURN_FROM_CALLEE_LABEL)
            self.comrv_ret_from_callee_context_switch \
                = get_symbol_address (COMRV_RETURN_FROM_CALLEE_CONTEXT_SWITCH_LABEL)
            self.comrv_igonr_caller_thunk_stack_frame \
                = get_symbol_address (COMRV_IGONR_CALLER_THUNK_STACK_FRAME)
            self.comrv_entry \
                = get_symbol_address (COMRV_ENTRY_LABEL)
            self.comrv_end \
                = get_symbol_address (COMRV_END_LABEL)
            self.comrv_entry_context_switch \
                = get_symbol_address (COMRV_ENTRY_CONTEXT_SWITCH_LABEL)
            self.comrv_exit \
                = get_symbol_address (COMRV_EXIT_LABEL)
            self.enabled = (self.comrv_invoke_callee
                            and  self.ret_from_callee
                            and self.comrv_entry and self.comrv_exit)

    # A wrapper class to hold all the different information we loaded from
    # target memory.  An instance of this is what we return from the fetch
    # method.
    class _overlay_data_inner:
        def __init__ (self, cache_descriptor, storage_descriptor, groups_data,
                      mg_index_offset, info_sym):
            self._cache_descriptor = cache_descriptor
            self._groups_data = groups_data
            self._storage_descriptor = storage_descriptor
            self._multi_group_index_offset = mg_index_offset
            self._info_sym = info_sym

        def cache (self):
            return self._cache_descriptor

        def storage (self):
            return self._storage_descriptor

        def group (self, index):
            return self._groups_data.get_group (index)

        def group_count (self):
            return self._groups_data.get_group_count ()

        def multi_group (self, index):
            return self._groups_data.get_multi_group (index)

        def multi_group_count (self):
            return self._groups_data.get_multi_group_count ()

        def is_multi_group_enabled (self):
            return self._multi_group_index_offset > 0

        def multi_group_index_offset (self):
            return self._multi_group_index_offset

        def get_token_from_multi_group_table (self, index):
            return self._groups_data.get_token_from_multi_group_table (index)

        def comrv_initialised (self):
            return (not self._groups_data == None)

        def labels (self):
            # TODO: Maybe we could do some caching here?
            return overlay_data._comrv_labels ()

        def comrv_info (self):
            return self._info_sym

    # Read the group offset for overlay group GROUP_NUMBER.  The
    # overlay data starts at address BASE_ADDRESS in memory.
    #
    # Return the offset in bytes for the specified overlay group.
    @staticmethod
    def _read_overlay_offset (base_address, end_address, group_number):
        base_address = base_address + (2 * group_number)
        if ((base_address + 1) >= end_address):
            raise RuntimeError ("out of bounds access while reading offset "
                                + "table for group %d" % (group_number))
        scaled_offset = mem_reader.read_16_bit (base_address)
        offset = get_comrv_min_entry_size () * scaled_offset
        return offset

    # Read a 32-bit overlay token from the multi-group table.  ADDRESS
    # is the exact address from which the token should be loaded.
    @staticmethod
    def _read_overlay_token (address):
        token = mem_reader.read_32_bit (address)
        return token

    # Load information about all of the groups and multi-groups from the
    # overlay cache tables, and return an instance of an object holding all of
    # this data.
    @staticmethod
    def _load_group_data (table_start, table_size, storage_desc,
                          multi_group_offset):

        def _load_overlay_groups (table_start, table_end, storage_start):
            groups = list ()

            # Read all of the overlay group offsets from memory, adding
            # entries to the overlay group list as we go.
            grp = 0

            # Read the offset of the very first overlay group.  This
            # should always be 0, but lets check it anyway.
            prev_offset \
                = overlay_data._read_overlay_offset (table_start,
                                                     table_end,
                                                     grp)
            if (prev_offset != 0):
                raise RuntimeError ("offset of first overlay group is 0x%x not 0"
                                    % (prev_offset))
            while (True):
                # Read the offset for the start of the next overlay group.
                next_offset \
                    = overlay_data._read_overlay_offset (table_start,
                                                         table_end,
                                                         (grp + 1))

                # An offset of 0 indicates the end of the group table.
                if (next_offset == 0):
                    break

                # Calculate the size of this overlay group, and create an
                # object to represent it.
                size = next_offset - prev_offset
                groups.append (overlay_data.
                               _overlay_group (storage_start + prev_offset,
                                               size, grp))
                grp += 1
                prev_offset = next_offset

            return groups

        def _mg_members_to_func_and_size (id, members):
            mg_block = None
            for m in members:
                addr = m.overlay_group.base_address () + m.offset
                #size = size_of_function_at (addr)
                b = function_block_at (addr)
                if (mg_block == None):
                    mg_block = b
                elif (b != None and b != mg_block):
                    raise RuntimeError ("multiple sizes for multi-group %d" % id)
            if (mg_block == None):
                raise RuntimeError ("unable to find size of multi-group %d" % id)
            mg_name = None
            mg_size = None
            if (mg_block != None):
                if (mg_block.function != None):
                    mg_name = mg_block.function.name
                mg_size = mg_block.end - mg_block.start
            return (mg_name, mg_size)

        def _load_overlay_multi_groups (table_start, table_end, overlay_groups):
            multi_groups = list ()
            all_tokens = list ()

            # The start and end of the region containing the
            # multi-group table.
            mg_start = table_start
            mg_end = table_end

            # A number assigned to each multi-group.  Starts at 0, and
            # increments by one for each multi-group.
            mg_num = 0

            # An index assigned to each multi-group.  This is the
            # index of the first member of the multi-group.
            mg_idx = 0

            # Used to track the index into the multi-group table.
            idx = 0

            # The tokens within the current multi-group.
            mg_tokens = list ()

            while (mg_start < mg_end):
                # Read a 32-bit overlay token from the multi-group table.
                ovly_token = overlay_data._read_overlay_token (mg_start)
                all_tokens.append (ovly_token)
                idx += 1

                # A token of 0 indicates the end of a multi-group.
                if (ovly_token == 0):
                    # If this is the first entry in a multi-group then we
                    # have reached the end of all multi-group data, and
                    # we're done.
                    if (len (mg_tokens) == 0):
                        break
                    # Otherwise, we've reached the end of this
                    # multi-group, but there might be more after this.
                    # Finalise this multi-group, and prepare to parse the
                    # next.
                    else:
                        # Take TOKEN, a non-multi-group token
                        # extracted from the multi-group table, and
                        # return a new multi-group member object.
                        def token_to_member (token):
                            g = overlay_token_group_id (token)
                            og = overlay_groups[g]
                            return overlay_data.\
                                _overlay_multi_group_member (og, token)

                        # Convert MG_TOKENS, a list of all the
                        # non-multi-group tokens that are within this
                        # multi-group, into a list of multi-group
                        # member objects (in MG_MEMBERS).
                        mg_members = map (token_to_member, mg_tokens)
                        (mg_func, mg_size) \
                            = _mg_members_to_func_and_size (mg_num, mg_members)
                        multi_groups.append \
                            (overlay_data._overlay_multi_group \
                             (mg_num, mg_idx, mg_members, mg_size,
                              mg_func))
                        # Now reset ready to read the next multi-group.
                        mg_num += 1
                        mg_idx = idx
                        mg_tokens = list ()
                # Otherwise a non-zero token is a member of the multi-group.
                else:
                    mg_tokens.append (ovly_token)
                mg_start += 4		# The size of one overlay token.
            return multi_groups, all_tokens

        storage_start = storage_desc.start_address ()
        if (multi_group_offset >= 0):
            table_end = table_start + multi_group_offset
        else:
            table_end = table_start + table_size

        groups = _load_overlay_groups (table_start,
                                       table_end,
                                       storage_start)

        if (multi_group_offset >= 0):
            table_end = table_start + table_size
            table_start += multi_group_offset
            multi_groups, all_tokens \
                = _load_overlay_multi_groups (table_start, table_end, groups)
        else:
            multi_groups = list ()
            all_tokens = list ()

        return (overlay_data.
                _overlay_group_data (groups, multi_groups, all_tokens))

    # Read the address of symbol NAME from the inferior, return the
    # address as an integer.  If an error is thrown (missing symbol?)
    # then None is returned.
    @staticmethod
    def _read_symbol_address_as_integer (name):
        return get_symbol_address (name)

    # Read the value of symbol NAME from the inferior, return the
    # value as an integer.  If the symbol can't be read (missing
    # symbol?) then return None.
    @staticmethod
    def _read_symbol_value_as_integer (name):
        try:
            return int (gdb.parse_and_eval ("%s" % (name)))
        except:
            return None

    # Load from target memory information about the overlay cache and the
    # overlay groups.
    @staticmethod
    def fetch ():
        if (overlay_data._instance != None):
            return overlay_data._instance

        # The overlay cache is defined by two symbols, a start and end
        # symbol.  Read these and create a cache descriptor object.
        cache_start = overlay_data.\
                      _read_symbol_address_as_integer \
				(OVERLAY_CACHE_START_SYMBOL)
        cache_end = overlay_data.\
                    _read_symbol_address_as_integer \
				(OVERLAY_CACHE_END_SYMBOL)
        if (cache_start and cache_end):
            cache_desc = overlay_data._cache_descriptor (cache_start, cache_end)
        else:
            cache_desc = None

        # Similarly, the storage area, where overlays are loaded from, is
        # defined by a start and end symbol.
        storage_start = overlay_data.\
                        _read_symbol_address_as_integer \
                (OVERLAY_STORAGE_START_SYMBOL)
        storage_end = overlay_data.\
                        _read_symbol_address_as_integer \
                (OVERLAY_STORAGE_END_SYMBOL)
        if (storage_start and storage_end):
            storage_desc \
                = overlay_data._storage_descriptor (storage_start, storage_end)
        else:
            storage_desc = None

        # This is the offset to the start of the multi-group table
        # from the start of the overlay tables.  We set this to -1
        # here, if this ComRV doesn't have multi-group support then
        # this is left as -1.
        multi_group_offset = -1


        # Finally, if ComRV has been initialised then load the current state
        # from memory.
        init_been_called = global_has_comrv_been_initialised_yet ()
        if (init_been_called):
            try:
                multi_group_offset = overlay_data.\
            _read_symbol_value_as_integer (MULTI_GROUP_OFFSET_SYMBOL)
                # The multi-group offset is held in the number of
                # 2-byte chunks, so convert this into a byte offset.
                multi_group_offset *= 2
            except:
                pass
            # read the overlay info value
            info_sym = overlay_data.\
                      _read_symbol_value_as_integer (COMRV_INFO_SYMBOL)
            if (info_sym == None):
                raise RuntimeError ("Couldn't read info symbol `%s'"
                                   % COMRV_INFO_SYMBOL)
            groups_data = overlay_data.\
                          _load_group_data (cache_desc.tables_base_address (),
                                            cache_desc.tables_size_in_bytes (),
                                            storage_desc, multi_group_offset)
        else:
            groups_data = None
            info_sym = None

        # Work out the size in bits of the multi-group index on the comrv stack.
        # A size of zero means this ComRV does not have multi-group support.
        if multi_group_offset > 0:
          multi_group_index_offset = info_sym & 0xF
          if (multi_group_index_offset not in [11, 14]):
              raise RuntimeError ("Invalid multi-group index offset (expected "
                  + " 11 or 14, but got " + str(multi_group_index_offset) + ")")
        else:
          multi_group_index_offset = 0

        # Now package all of the components into a single class
        # instance that we return.  We only cache the object if ComRV
        # has been initialised, in this way we shouldn't get stuck
        # with a cached, not initialised object.
        obj = overlay_data._overlay_data_inner (cache_desc, storage_desc,
                                                groups_data,
                                                multi_group_index_offset,
                                                info_sym)
        if (init_been_called):
            overlay_data._instance = obj
        return obj

    # Discard the information loaded from the cache.  The next time fetch is
    # called the information will be reread.
    @staticmethod
    def clear ():
        overlay_data._instance = None

# Class for walking the overlay data structures and calling the
# visit_mapped_overlay method for every mapped overlay group.
class mapped_overlay_group_walker:

    class eviction_lru(object):

        def __init__(self):
            self.eviction_values = []

        def read_values(self):
            # get the lru and mru values
            lru = gdb.parse_and_eval(OVERLAY_TABLE_ENTRY_LRU_INDEX_VAL)
            mru = gdb.parse_and_eval(OVERLAY_TABLE_ENTRY_MRU_INDEX_VAL)
            mru = int(mru)
            lru = int(lru)
            # this is a case where cache is fully ocupied with one 
            # group - so lru and mru point to the same location
            if lru == mru:
                self.eviction_values.append(0)
            else:
                # walk trouogh lru list and save eviction index 
                while (lru != 255):
                    self.eviction_values.append(lru)
                    lru = gdb.parse_and_eval(OVERLAY_TABLE_ENTRY_LRU_EVICTION_VAL % (lru))
                    lru = int(lru)

        def get_eviction_value(self, index):
            return self.eviction_values.index(index)


    class eviction_factory(object):

        def __init__(self, eviction_type):
            if eviction_type == COMRV_INFO_EVICT_POLICY_LRU:
                self.evict_obj = mapped_overlay_group_walker.eviction_lru()
            else:
                raise RuntimeError ("Unknown eviction type")
            if not getattr(self.evict_obj, "read_values", None):
                raise RuntimeError ("missing read_values implementation")
            if not getattr(self.evict_obj, "get_eviction_value", None):
                raise RuntimeError ("missing get_eviction_value implementation")
            self.evict_obj.read_values()

        def get_eviction_value(self, index):
            return self.evict_obj.get_eviction_value(index)


    # Call this to walk the overlay manager data structures in memory and
    # call the visit_mapped_overlay method for each mapped overlay group.
    def walk_mapped_overlays (self):
        # Grab the data that describes the current overlay state.
        ovly_data = overlay_data.fetch ()
        if (not ovly_data.comrv_initialised ()):
            self.comrv_not_initialised ()
            return

        # Now walk the overlay cache and see which entries are mapped in.
        index = 0
        # read eviction values list
        evict_obj = self.eviction_factory(ovly_data.comrv_info() & COMRV_INFO_EVICTION_FIELD)
        
        while (index < ovly_data.cache ().number_of_working_entries ()):
            group = gdb.parse_and_eval (OVERLAY_CACHE_AT_INDEX_TO_GROUP_ID % (index))
            group = int (group)
            offset = None

            if (group != 0xffff):
                # Found an entry that is mapped in.
                group_desc = ovly_data.group (group)
                src_addr = group_desc.base_address ()
                length = group_desc.size_in_bytes ()
                dst_addr = (ovly_data.cache ().base_address ()
                            + (index
                               * ovly_data.cache ().entry_size_in_bytes ()))


                # get entry token
                token_val = gdb.parse_and_eval (OVERLAY_TABLE_ENTRY_TOKEN_VAL % (index))
                token_val = int (token_val)
                # get cache entry evict lock property
                evict_lock = gdb.parse_and_eval (OVERLAY_TABLE_ENTRY_EVICT_LOCK_VAL % (index))
                evict_lock = int (evict_lock)
                # get cache entry lock property
                entry_lock = gdb.parse_and_eval (OVERLAY_TABLE_ENTRY_GROUP_LOCK_VAL % (index))
                entry_lock = int (entry_lock)
                # get cache data property
                data = gdb.parse_and_eval (OVERLAY_TABLE_ENTRY_DATA_OVL_VAL % (index))
                data = int (data)
                if (not self.visit_mapped_overlay (src_addr, dst_addr, length,
                                                index, group, evict_lock, entry_lock,
                                                evict_obj.get_eviction_value(index), data, token_val)):
                    break

                offset = gdb.parse_and_eval (OVERLAY_CACHE_AT_INDEX_TO_SIZE_IN_MIN_UNITS % (index))
                offset = int (offset)
                if (offset == 0):
                    # Something has gone wrong here.  An overlay
                    # appears to be mapped, but has 0 size.  Maybe we
                    # could load the overlay size from the static
                    # data, after all we do know it.  For now just
                    # force to 1 so we don't get stuck.
                    offset = 1
            else:
                # Found an entry that is not currently mapped - get the entry size
                offset = gdb.parse_and_eval (OVERLAY_CACHE_AT_INDEX_TO_SIZE_IN_MIN_UNITS % (index))
                offset = int (offset)

            # Move to the next cache entry.
            index += offset

    # Default implementation of visit_mapped_overlay, sub-classes should
    # override this method.  Return true to continue walking the list of
    # mapped overlays, or return false to stop.
    def visit_mapped_overlay (self, src_addr, dst_addr, length,
                              cache_index, group_number, evict_lock = 0,
                              entry_lock = 0, evict_value = 0, data = 0, 
                              token_val = 0):
        return True

    # Default implementation of comrv_not_initialised, sub-classes
    # should override this method.  This is called if
    # walk_mapped_overlays is called before ComRV is initialised.
    def comrv_not_initialised (self):
        None

def print_current_comrv_state ():
    ovly_data = overlay_data.fetch ()
    if (not ovly_data.comrv_initialised ()):
        print ("ComRV not yet initialisd:")
        return

    print ("Overlay Regions:")
    print ("  %-9s%-12s%-12s%-8s" % ("Region", "Start", "End", "Size"))
    print ("  %-9s0x%-10x0x%-10x0x%-6x"
           % ("storage",
              ovly_data.storage ().start_address (),
              ovly_data.storage ().end_address (),
              (ovly_data.storage ().end_address () -
               ovly_data.storage ().start_address ())))
    print ("  %-9s0x%-10x0x%-10x0x%-6x"
           % ("cache",
              ovly_data.cache ().start_address (),
            ovly_data.cache ().end_address (),
              (ovly_data.cache ().end_address () -
               ovly_data.cache ().start_address ())))
    print ("")
    print ("Overlay groups:")
    grp_num = 0
    while (grp_num < ovly_data.group_count ()):
        grp = ovly_data.group (grp_num)
        if (grp == None):
            break
        if (grp_num == 0):
            print ("  %-7s%-12s%-12s%-8s" % ("Group", "Start", "End", "Size"))
        print ("  %-7d0x%-10X0x%-10X0x%-6X"
               % (grp_num, grp.base_address (),
                  (grp.base_address () + grp.size_in_bytes ()),
                  grp.size_in_bytes ()))
        grp_num += 1
    print ("")
    print ("Overlay multi-groups:")
    if (ovly_data.is_multi_group_enabled ()):
        for grp_num in range (0, ovly_data.multi_group_count ()):
            mg = ovly_data.multi_group (grp_num)
            if (grp_num == 0):
                print ("  %6s%-7s%-12s%-9s%-10s%-10s%-10s"
                       % ("", "", "", "Overlay", "Function",
                          "Function", "Function"))
                print ("  %-6s%-7s%-12s%-9s%-10s%-10s%-10s"
                       % ("Num", "Index", "Token", "Group", "Offset",
                          "Size", "Name"))
            else:
                print ("  %-6s%-7s%-12s%-9s%-10s%-10s%-10s"
                       % ("---", "---", "---", "---", "---", "---",
                          "---"))
            for m in mg.members:
                print ("  %-6d%-7d0x%08x  %-9d0x%-8x0x%-8x%s"
                       % (grp_num, mg.index (), m.token,
                          m.overlay_group.id, m.offset,
                          mg.size_in_bytes, mg.function_name))
    else:
        print ("  Not supported in this ComRV build.")
    print ("")
    print ("Current overlay mappings:")
    # Class to walk the currently mapped overlays and print a summary.
    class print_mapped_overlays (mapped_overlay_group_walker):
        def __init__ (self):
            self._shown_header = False
            self.walk_mapped_overlays ()
            if (not self._shown_header):
                self.nothing_is_mapped ()

        def visit_mapped_overlay (self, src_addr, dst_addr, length,
                                  cache_index, group_number, evict_lock,
                                  entry_lock, evict_value, data, token_val):
            if (not self._shown_header):
                self._shown_header = True
                print ("  %-7s%-9s%-12s%-12s%-9s%-7s%-7s%-12s%-9s%-12s"
                       % ("Cache", "Overlay", "Storage", "Cache", "Group", "Evict", "Entry", "Evict", "Data", "Token"))
                print ("  %-7s%-9s%-12s%-12s%-9s%-7s%-7s%-12s%-9s%-12s"
                       % ("Index", "Group",   "Addr",    "Addr",  "Size",  "Lock",  "Lock", "Value", "Overlay", "Addr"))

            print ("  %-7d%-9d0x%-10X0x%-10X0x%-7X%-7d%-7d0x%-10X%-9d0x%-10X"
                   % (cache_index, group_number, src_addr, dst_addr, length, 
                      evict_lock, entry_lock, evict_value, data, token_val))
            return True

        def nothing_is_mapped (self):
            print ("  No overlays groups are currently mapped.")

    print_mapped_overlays ()

# Model a single frame on the ComRV stack.
class comrv_stack_frame:
    def __init__ (self, addr, mg_index_offset):
        self._frame_addr = addr
        self._return_addr = mem_reader.read_32_bit (addr)
        self._token = mem_reader.read_32_bit (addr + 4)
        self._offset = mem_reader.read_16_bit (addr + 8)
        self._align = mem_reader.read_8_bit (addr + 10)
        if (mg_index_offset > 0):
            if (mg_index_offset == 11):
                index = mem_reader.read_8_bit (addr + 11)
                self._mg_index = sign_extend (index, 8)
            else:
                index = mem_reader.read_16_bit (addr + 14)
                self._mg_index = sign_extend (index, 16)
        else:
            self._mg_index = 0

    def frame_address (self):
        return (self._frame_addr & 0xffffffff)

    def return_address (self):
        return (self._return_addr & 0xffffffff)

    def token (self):
        return (self._token & 0xffffffff)

    def align (self):
        return ((self._align & 0xffff) * get_comrv_min_entry_size ())

    def multi_group_index (self):
        return (self._mg_index & 0xffff)

    def offset (self):
        return self._offset

class comrv_prefix_command (gdb.Command):
    def __init__ (self):
        gdb.Command.__init__ (self, "comrv", gdb.COMMAND_NONE, gdb.COMPLETE_NONE, True)

# The class represents a new GDB command 'comrv status' that reads the current
# overlay status and prints a summary to the screen.
class comrv_status_command (gdb.Command):
    '''Display the current state of ComRV overlays.

This command only works once ComRV has been initialised.

The information displayed includes the addresses of the ComRV
cache and storage areas, a summary of all the groups and multi-groups
as well as which overlay groups are currently mapped in.'''
    def __init__ (self):
        gdb.Command.__init__ (self, "comrv status", gdb.COMMAND_NONE)

    def invoke (self, args, from_tty):
        print_current_comrv_state ()

        # Discard the cached cache data, incase we ran this command at the
        # wrong time and the cache information is invalid.  This will force
        # GDB to reload the information each time this command is run.
        overlay_data.clear ()

# The class represents a new GDB command 'comrv stack' that reads the
# current ComRV stack, and prints a summary.  This is related to, but
# not the same as backtracing, as the backtrace interprets the ComRV
# stack, while this is a raw peek into the stack.
class comrv_stack_command (gdb.Command):
    '''Display the ComRV stack.

This is different to the normal GDB backtrace in that backtrace interprets
the ComRV stack, while this command just dumps the raw stack contents.

This requires ComRV to be initialised before any stack can be displayed.

The fields are:
    Frame - The frame number, just an index, with the lowest number being
            the most recent frame.
  Address - The address of the frame, that is the value of register t3 that
            points to this frame.
      R/A - The return address field for this stack frame.
    Token - The token field for this stack frame.
Alignment - The alignment field from the ComRV stack, alignment to size of
            maximum group size.
      M/G - (Only in multi-group builds of ComRV) The overlay group token
            for the specific overlay that was used.
     Size - The size of this stack frame.  The outermost frame should have
            a size of 0xdead, this indicates the end of the stack.'''

    def __init__ (self):
        gdb.Command.__init__ (self, "comrv stack", gdb.COMMAND_NONE)

    def invoke (self, args, from_tty):
        ovly_data = overlay_data.fetch ()
        is_initialised = ovly_data.comrv_initialised ()
        is_mg = ovly_data.is_multi_group_enabled ()
        mg_index_offset = ovly_data.multi_group_index_offset ()
        overlay_data.clear ()

        if (not is_initialised):
            print ("ComRV not yet initialised")
            return

        t3_addr = int (gdb.parse_and_eval ("$t3"))
        depth = 0
        if (is_mg):
            print ("%5s %10s %10s %10s %10s %6s %6s"
                   % ("Frame", "Address", "R/A", "Token", "Alignment", "M/G", "Size"))
        else:
            print ("%5s %10s %10s %10s %10s %6s"
                   % ("Frame", "Address", "R/A", "Token", "Alignment", "Size"))
        while (True):
            frame = comrv_stack_frame (t3_addr, mg_index_offset)
            if (is_mg):
                print ("%5s %10s %10s %10s %10s %6s %6s"
                       % (("#%d" % (depth)),
                          ("0x%08x" % (frame.frame_address ())),
                          ("0x%08x" % (frame.return_address ())),
                          ("0x%08x" % (frame.token ())),
                          ("0x%08x" % (frame.align ())),
                          ("0x%04x" % (frame.multi_group_index ())),
                          ("0x%x" % (frame.offset ()))))
            else:
                print ("%5s %10s %10s %10s %10s %6s"
                       % (("#%d" % (depth)),
                          ("0x%08x" % (frame.frame_address ())),
                          ("0x%08x" % (frame.return_address ())),
                          ("0x%08x" % (frame.token ())),
                          ("0x%08x" % (frame.align ())),
                          ("0x%x" % (frame.offset ()))))
            depth += 1
            if (frame.offset () == 0xdead or frame.offset () == 0x0):
                break
            t3_addr += frame.offset ()


# The command 'parse-comrv' existed once, but is now deprecated.
class ParseComRV (gdb.Command):
    'Parse the ComRV data table.'

    def __init__ (self):
        gdb.Command.__init__ (self, "parse-comrv", gdb.COMMAND_NONE)

    def invoke (self, args, from_tty):
        raise RuntimeError ("this command is deprecated, use 'comrv status' instead")

# The class represents a new GDB command 'comrv group <LOC>' that
# takes a location specifier, as taken by the 'break' command, and
# reports which groups that location is in.
class comrv_groups_command (gdb.Command):
    '''Display the overlay groups a particular location is in.

This command only works once ComRV has been initialised.

Takes a single argument that is a string describing a location in the program
being debugged, in the same format as the breakpoint command.  This location
is translated to an address (or multiple addresses), and then the group or
groups those addresses appear in are listed.'''
    def __init__ (self):
        gdb.Command.__init__ (self, "comrv groups", gdb.COMMAND_NONE,
                              gdb.COMPLETE_LOCATION)

    # If ADDR is inside a multi-group then return a list of all the
    # storage area addresses that are duplicates of ADDR.  Otherwise
    # return a single entry list containing just ADDR.
    def _expand_mg_addresses (self, addr, ovly_data):
        # If we have multi-groups in this program, we need to expand
        # them now and figure out if our address is in any of them.
        for i in range (0, ovly_data.multi_group_count ()):
            mg = ovly_data.multi_group (i)
            for m in mg.members:
                low = m.overlay_group.base_address () + m.offset
                high = low + mg.size_in_bytes
                if (addr >= low and addr < high):
                    # Is in this multi-group!
                    offset = addr - low
                    return map (
                        lambda x : (x.overlay_group.base_address ()
                                    + x.offset + offset),
                        mg.members)

        # Not in any multi-groups.
        return [addr]

    # Find an overlay group containing ADDR and return it, otherwise, return
    # None.
    def _find_group (self, ovly_data, addr):
        for grp_num in range (0, ovly_data.group_count ()):
            grp = ovly_data.group (grp_num)
            if (addr >= grp.base_address ()
                and addr < (grp.base_address () + grp.size_in_bytes ())):
                return (grp, grp_num)
        return (None, None)

    def invoke (self, args, from_tty):
        ovly_data = overlay_data.fetch ()
        if (not ovly_data.comrv_initialised ()):
            print ("ComRV not yet initialisd:")

        if (args == ""):
            raise RuntimeError ("missing location argument");
        (junk,sal) = gdb.decode_line (args)
        if (junk != None):
            raise RuntimeError ("junk at end of line: %s" % (junk))

        print ("%-12s%-7s%-12s%-12s%-8s%-8s"
               % ("", "Group", "Group", "Group", "Group", ""))
        print ("%-12s%-7s%-12s%-12s%-8s%-8s"
               % ("Address", "Number", "Start", "End", "Size", "Offset"))
        for s in (sal):
            pc = s.pc
            if (pc >= ovly_data.storage ().start_address ()
                and pc < ovly_data.storage ().end_address ()):
                all_addresses = self._expand_mg_addresses (pc, ovly_data)
                for addr in (all_addresses):
                    # Figure out which overlay group address ADDR is in.
                    (grp, idx) = self._find_group (ovly_data, addr)
                    if (grp == None):
                        print ("0x%-10x\t** not in an overlay group **"
                               % (addr))
                    else:
                        print ("0x%-10x%-7d0x%-10x0x%-10x0x%-6x0x%-6x"
                               % (addr, idx, grp.base_address (),
                                  (grp.base_address () + grp.size_in_bytes ()),
                                  grp.size_in_bytes (),
                                  (addr - grp.base_address ())))
            else:
                print ("0x%-10x\t** is not in overlay storage area **"
                       % (pc))

        # Discard the cached cache data, incase we ran this command at the
        # wrong time and the cache information is invalid.  This will force
        # GDB to reload the information each time this command is run.
        overlay_data.clear ()

class MyOverlayManager (gdb.OverlayManager):
    def __init__ (self):
        gdb.OverlayManager.__init__ (self, True)
        # STOP !
        #
        # No code should be placed here that assumes the ELF being
        # debugged is currently loaded.  It is highly likely that this
        # file is sourced before the ELF being debugged is loaded (for
        # example in Eclipse) in which case non of the required
        # symbols will exist.
        pass

    def get_region_data (self):
        ovly_data = overlay_data.fetch ()
        debug ("Setting up overlay manager region data:")
        debug ("Cache:")
        debug ("  Start: 0x%x" % (ovly_data.cache ().start_address ()))
        debug ("    End: 0x%x" % (ovly_data.cache ().end_address ()))
        debug ("Storage:")
        debug ("  Start: 0x%x" % (ovly_data.storage ().start_address ()))
        debug ("    End: 0x%x" % (ovly_data.storage ().end_address ()))
        self.set_storage_region (ovly_data.storage ().start_address (),
                                 ovly_data.storage ().end_address ())
        self.set_cache_region (ovly_data.cache ().start_address (),
                               ovly_data.cache ().end_address ())

    def __del__ (self):
        print ('Destructor called for MyOverlayManager')

    # Return a string, where GDB should place the overlay event
    # breakpoint.
    def event_symbol_name (self):
        debug ("In Python code, event_symbol_name")
        return "_ovly_debug_event"

    # Return an integer, the number of multi-groups.  Return the
    # special value -1 to indicate ComRV is not yet initialised, and
    # so we don't know how many multi-groups there are, in this case
    # GDB will ask again later.  Otherwise return a value greater
    # than, or equal to zero, GDB will cache this answer and not ask
    # again.
    def get_multi_group_count (self):
        debug ("In Python get_multi_group_count method")
        mg_count = -1
        ovly_data = overlay_data.fetch ()
        if (ovly_data.comrv_initialised ()):
            mg_count = ovly_data.multi_group_count ()
        debug ("In Python get_multi_group_count method = %d" % (mg_count))
        return mg_count

    # For multi-group number ID return a list of all the storage area
    # addresses of all the functions within this multi-group.
    def get_multi_group (self, id):
        ovly_data = overlay_data.fetch ()
        if (not ovly_data.comrv_initialised ()):
            raise RuntimeError ("ComRV not yet initialised")
        if (id >= ovly_data.multi_group_count ()):
            raise RuntimeError ("Multi-group index out of range")
        res = list ()
        mg = ovly_data.multi_group (id)
        for m in mg.members:
            addr = m.overlay_group.base_address () + m.offset
            res.append (addr)
        return res

    # Called to read the current state of ComRV, which overlays are
    # mapped in.  Should call the ADD_MAPPING method on ourselves
    # (implemented inside GDB) to inform GDB about an active overlay
    # mapping.
    def read_mappings (self):
        debug ("In Python code, read_mappings")

        # If we're reading mappings then ComRV must be initialised.
        global_mark_comrv_as_initialised ()

        global overlay_debug
        if (overlay_debug):
            print_current_comrv_state ()

        # Class to walk mapped overlays and add them to the list of currently
        # mapped overlays.
        class map_overlays (mapped_overlay_group_walker):
            def __init__ (self, manager):
                self._manager = manager
                self.walk_mapped_overlays ()

            def visit_mapped_overlay (self, src_addr, dst_addr, length,
                                      cache_index, group_number, evict_lock = 0,
                                      entry_lock = 0, evict_value = 0, data = 0,
                                      token_val = 0):
                self._manager.add_mapping (src_addr, dst_addr, length)
                return True

        # Create an instance of the previous class, this does all the work in
        # its constructor.
        map_overlays (self)

        debug ("All mappings added")
        return True

    # Return the base address, within the storage area, for overlay
    # group ID.  The base address is the first address of an overlay
    # group.
    def get_group_storage_area_address (self, id):
        debug ("get_group_storage_area_address (%d) = ..." % (id))
        ovly_data = overlay_data.fetch ()
        if (not ovly_data.comrv_initialised ()):
            raise RuntimeError ("ComRV not initialised, overlay "
                                + "storage area address unknown")

        group_desc = ovly_data.group (id)
        tmp = group_desc.base_address ()

        debug ("get_group_storage_area_address (%d) = 0x%x"
               % (id, tmp))
        return tmp

    # Get the callee that the overlay manager is calling.  This method should
    # only be called when the pc is at one of the comrv entry points for a call.
    def get_callee_primary_storage_area_address (self):
        global_mark_comrv_as_initialised ()

        ovly_data = overlay_data.fetch ()
        if (not ovly_data.comrv_initialised ()):
            raise RuntimeError ("ComRV is not initialised")

        # Assert pc is at one of the comrv entry points for a call.
        labels = ovly_data.labels()
        pc = int (gdb.parse_and_eval ("$pc"))
        assert (pc in [labels.comrv_entry,
                       labels.comrv_entry_context_switch])

        token = int (gdb.parse_and_eval ("$t5"))

        if (not is_overlay_token_p (token)):
          # The callee is a non-overlay function and token is destination
          # address.
          return token;

        if is_multi_group_token_p (token):
            multi_group_id = mg_token_group_id (token)
            token = ovly_data.get_token_from_multi_group_table (multi_group_id)

        # TOKEN is now a non-multi-group token.
        assert (not is_multi_group_token_p (token))
        group_id = overlay_token_group_id (token)
        func_offset = overlay_token_func_offset (token)

        ba = self.get_group_storage_area_address (group_id);
        addr = ba + func_offset;

        return addr

#=====================================================================#
#                         Frame Filters
#=====================================================================#
#
# The frame filter modifies how GDB displays certain frames in the
# backtrace.
#
# As the assembler core of ComRV is split up into parts by the
# different global labels, normally GDB would display a different
# function name depending on which part of the ComRV core you are in.
#
# However, we install a frame filter that groups all of these parts
# together and labels them all as 'comrv'.
#
# The frame filter can also provide, or modify, the arguments that are
# displayed for a particular frame.  For ComRV we create a single
# pseudo-argument 'token', in which we try to display the token that
# was passed in to ComRV.
#
# Obviously, we can't always figure out the ComRV token, for example,
# after calling the callee, during the return phase, the previous
# token is gone.  In this case we return an optimised out value for
# the token.  The intention is that _if_ GDB displays a token value,
# then it should be the correct token value.
#
# The user command 'set comrv show-token on|off' can be used to
# control whether GDB displays the pseudo token parameter or not.
#
#=====================================================================#

class comrv_frame_filter ():
    """
    A class for filtering ComRV stack frame entries.

    This class does one of two jobs based on the current value of
    SHOW_COMRV_FRAMES.  When SHOW_COMRV_FRAMES is true then this class
    identifies ComRV stack frames and applies the DECORATOR sub-class
    to those frames.  When SHOW_COMRV_FRAMES is false this class
    causes the ComRV stack frames to be skipped so they will not be
    printed in the backtrace.
    """

    class decorator (FrameDecorator):
        """
        A FrameDecorator to change the name of the ComRV stack frames.

        This class is applied to ComRV stack frames when
        SHOW_COMRV_FRAMES is true, and changes the name of the frame
        to be simply "comrv".
        """

        def __init__(self, frame):
            FrameDecorator.__init__ (self, frame)
            self.uint_t = gdb.lookup_type ("unsigned int")
            self._frame = frame

        def function (self):
            return "comrv"

        def frame_args (self):
            '''Add pseudo-parameters to comrv frames.  When SHOW_COMRV_TOKENS is
            true this function returns a description of the 'token'
            parameter for the comrv frame.'''
            class _sym_value ():
                def __init__ (self, name, value):
                    self._name = name
                    self._value = value

                def symbol (self):
                    return self._name

                def value (self):
                    return self._value

            if (not show_comrv_tokens):
                return None

            def _get_token_from_comrv_stack (obj):
                # Find the token on the ComRV stack.
                t3 = obj._frame.inferior_frame ()\
                                .read_register ("t3").cast (obj.uint_t)
                t3 &= 0xfffffffe
                ovly_data = overlay_data.fetch ()
                mg_index_offset = ovly_data.multi_group_index_offset ()
                comrv_frame = comrv_stack_frame (t3, mg_index_offset)
                labels = ovly_data.labels ()
                assert (labels.ret_from_callee != None)
                while (comrv_frame.return_address () == labels.ret_from_callee
                       and comrv_frame.return_address () != 0
                       and comrv_frame.offset () != 0xdead):
                    t3 += comrv_frame.offset ()
                    comrv_frame = comrv_stack_frame (t3, mg_index_offset)
                token = comrv_frame.token ()
                return token

            addr = self._frame.address ()
            labels = overlay_data.fetch ().labels ()
            if (addr <= labels.comrv_entry_context_switch):
                token = self._frame.inferior_frame ().read_register ("t5")
            elif (addr < labels.comrv_igonr_caller_thunk_stack_frame):
                token = _get_token_from_comrv_stack (self)
            else:
                token = self.uint_t.optimized_out ()
            return [_sym_value ("token", gdb.Value (token).cast (self.uint_t))]

    class iterator ():
        """
        An iterator to wrap the default iterator and filter frames.

        An instance of this iterator is created around GDB's default
        FrameDecorator iterator.  As frames are extracted from this
        iterator, if the frame looks like a ComRV frame then we apply
        an extra decorator to it.
        """

        def __init__ (self, iter):
            self.iter = iter

        def __iter__(self):
            return self

        def next (self):
            """Called each time GDB needs the next frame.  If the frame
            looks like a ComRV frame (based on its $pc value) then we
            either apply the comrv frame decorator, or we skip the
            frame (based on the value of SHOW_COMRV_FRAMES)."""
            frame = next (self.iter)
            addr = frame.address ()
            labels = overlay_data.fetch ().labels ()
            if (addr >= labels.comrv_entry
                and addr <= labels.comrv_end):
                if (not show_comrv_frames
                    and (frame.inferior_frame ()
                         != gdb.selected_frame ())):
                    return next (self.iter)
                else:
                    return comrv_frame_filter.decorator (frame)
            return frame

        def __next__ (self):
            return self.next ()

    def __init__ (self):
        self.name = "comrv filter"
        self.priority = 100
        self.enabled = True
        gdb.frame_filters [self.name] = self

    def filter (self, frame_iter):
        return self.iterator (frame_iter)

# Register the frame filter.
comrv_frame_filter ()

#=====================================================================#
#                    Disassembly Analysis
#=====================================================================#
#
# The following provides a mechanism for analysing assembly code at a
# very basic level.  These utilities are used by the stack unwinders.
# These analysis routines have just enough logic to analyse the ComRV
# assembler core, and are not sufficient for general assembler analysis.
#
#=====================================================================#

class pv_type:
    @property
    def type (self):
        return self.__class__.__name__

class pv_register (pv_type):
    def __init__ (self, name, addend = 0):
        self._name = name
        self._addend = addend

    def __str__ (self):
        if (self._addend > 0):
            return "PV_register (%s + %d)" % (self._name, self._addend)
        elif (self._addend < 0):
            return "PV_register (%s - %d)" % (self._name, abs (self._addend))
        else:
            return "PV_register (%s)" % (self._name)

    @property
    def addend (self):
        return self._addend

    @property
    def reg (self):
        return self._name

class pv_unknown (pv_type):
    def __init__ (self):
        pass

    def __str__ (self):
        return "PV_unknown ()"

class pv_constant (pv_type):
    def __init__ (self, imm):
        self._imm = int (imm)

    def __str__ (self):
        return "PV_constant (%d)" % (self._imm)

    @property
    def imm (self):
        return self._imm

def pv_add (src1, src2):
    if (src1.type == "pv_register"
        and src2.type == "pv_constant"):
        return pv_register (src1.reg, src1.addend + src2.imm)
    elif (src1.type == "pv_constant"
          and src2.type == "pv_constant"):
        return pv_constant (src1.imm + src2.imm)
    else:
        return pv_unknown ()

def pv_sub (src1, src2):
    if (src1.type == "pv_register"
        and src2.type == "pv_constant"):
        return pv_register (src1.reg, src1.addend - src2.imm)
    elif (src1.type == "pv_constant"
          and src2.type == "pv_constant"):
        return pv_constant (src1.imm - src2.imm)
    else:
        return pv_unknown ()

# INSN is a decoded_instruction, and REGS is a register_tracker.
def pv_simulate (insn, regs):
    if (insn.is_2_reg_insn ("mv")):
        regs[insn.rd] = regs[insn.rs1]
    elif (insn.mnemonic == "lw"
          or insn.mnemonic == "lh"
          or insn.mnemonic == "lb"):
        regs[insn.rd] = pv_unknown ()
    elif (insn.mnemonic in ["sw", "sh", "sb"]):
        # Ignore these instructions.
        pass
    elif (insn.is_3_reg_insn ("add")):
        regs[insn.rd] = pv_add (regs[insn.rs1], regs[insn.rs2])
    elif (insn.is_2_reg_and_imm_insn ("addi")):
        regs[insn.rd] = pv_add (regs[insn.rs1], pv_constant (insn.imm))
    elif (insn.is_3_reg_insn ("sub")):
        regs[insn.rd] = pv_sub (regs[insn.rs1], regs[insn.rs2])
    elif ((insn.mnemonic in ["andi", "ori", "slli"])
          and insn.rd != None):
        regs[insn.rd] = pv_unknown ()
    elif (insn.mnemonic in ["beqz", "bltz"]):
        # Ignore these instructions.
        pass
    elif (insn.mnemonic == "jal"):
        for r in ["ra", "a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7"]:
            regs[r] = pv_unknown ()
    else:
        print ("Unknown instruction: %s" % str (insn))

class register_tracker:
    def __init__ (self):
        self._regs = {}
        all_regs = ["x0", "ra", "sp", "gp", "tp", "t0", "t1", "t2", "s0", "s1",
                    "a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7",
                    "s2", "s3", "s4", "s5", "s6", "s7", "s8", "s9", "s10", "s11",
                    "t3", "t4", "t5", "t6"]
        self._reg_names = all_regs
        self._iter_index = 0
        for r in all_regs:
            self._regs[r] = pv_register (r)

    def __getitem__ (self, key):
        return self._regs[key]

    def __setitem__ (self, key, value):
        self._regs[key] = value

    def __iter__ (self):
        self._iter_index = 0
        return self

    def next (self):
        return self.__next__ ()

    def __next__ (self):
        try:
            r = self._reg_names[self._iter_index]
        except IndexError:
            raise StopIteration
        self._iter_index += 1
        return r

class decoded_instruction:
    def __init__ (self, insn):
        # Split the INSN so everything up to the first space is the
        # mnemonic, and everything after that is the operands.
        insn = insn.lstrip ().strip ()
        self._raw = insn
        (mnemonic, xxx, operands) = insn.partition ("\t")
        self._mnemonic = mnemonic
        self._rd = None
        self._rs1 = None
        self._rs2 = None
        self._imm = None

        # Now parse the operands.
        if (operands.find (",") == -1):
            # An immediate.
            (imm, sep, rest) = operands.partition (" ")
            self._imm = imm
            return

        (rd, sep, operands) = operands.partition (",")
        self._rd = rd

        if (operands.find ("(") != -1):
            # We have IMM(REG) remaining.
            (imm, sep, reg) = operands.partition ("(")
            (reg, sep, rest) = reg.partition (")")
            self._imm = imm
            self._rs1 = reg
            return

        if (operands.find (",") != -1):
            # We have REG,REG or REG,IMM remaining.
            (reg, sep, operands) = operands.partition (",")
            self._rs1 = reg

        # We should now have either REG or IMM left.
        m = re.match (r"^((?:0x[0-9a-f]+)|(?:-?[0-9]+))", operands)
        if (m):
            self._imm = m.group (0)
            return

        if (self._rs1 != None):
            self._rs2 = operands
        else:
            self._rs1 = operands

    def __str__ (self):
        return "[%s] MNEM=%s RD=%s, RS1=%s, RS2=%s, IMM=%s" % (
            self._raw, self._mnemonic, self._rd,
            self._rs1, self._rs2, self._imm
        )

    @property
    def mnemonic (self):
        return self._mnemonic

    @property
    def rd (self):
        return self._rd

    @property
    def rs1 (self):
        return self._rs1

    @property
    def rs2 (self):
        return self._rs2

    @property
    def imm (self):
        return self._imm

    def is_2_reg_insn (self, mnem):
        return (self.mnemonic == mnem
                and self.rd != None
                and self.rs1 != None)

    def is_2_reg_and_imm_insn (self, mnem):
        return (self.mnemonic == mnem
                and self.rd != None
                and self.rs1 != None
                and self.imm != None)

    def is_3_reg_insn (self, mnem):
        return (self.mnemonic == mnem
                and self.rd != None
                and self.rs1 != None
                and self.rs2 != None)

def comrv_disassemble_and_analyse (start, end):
    regs = register_tracker ()
    dis = gdb.execute (("disassemble 0x%x,0x%x" % (start, end)), False, True)
    dis = dis.splitlines ()
    found_header = False
    insns = []
    for l in (dis):
        if (l.startswith ("Dump of assembler code from ")):
            found_header = True
            continue
        elif (l.startswith ("End of assembler dump")):
            break
        elif (not found_header):
            continue
        elif (l.startswith ("=> ")):
            break
        (b,s,a) = l.partition (":")
        insns.append (decoded_instruction (a))

    for i in (insns):
        pv_simulate (i, regs)

    return regs

#=====================================================================#
#                         Stack Unwinder
#=====================================================================#
#
# The following stack unwinder performs unwinding for the ComRV
# assembler core.  The stack unwinder should be sufficient to unwind
# from any location within the assembler core.
#
#=====================================================================#

class comrv_unwinder (Unwinder):
    """
    A class to aid in unwinding through the ComRV engine.

    Implements GDB's Unwinder API in order to add support for
    unwinding through the ComRV engine.
    """

    class frame_id (object):
        """
        Holds information about a ComRV stack frame.

        An instance of this class is created for each identified ComRV
        stack frame.  The attributes of this class are as needed to
        satisfy GDB's frame unwinder API.
        """

        def __init__ (self, sp, pc):
            """Create an instance of this class, SP and PC are
            gdb.Value objects."""
            self.sp = sp
            self.pc = pc

    def __init__ (self):
        Unwinder.__init__ (self, "comrv stack unwinder")
        # If no executable is set, void pointer length will default to 8 bytes.
        # Setting a new executable may change this, so void_ptr_t is now updated
        # in __call__.
        self.void_ptr_t = gdb.lookup_type("void").pointer()

    def _get_multi_group_table_by_index (self, index):
        """Return the overlay token at position INDEX in the
        multi-group table."""

        ovly_data = overlay_data.fetch ()
        if (not ovly_data.comrv_initialised ()):
            raise RuntimeError ("ComRV not yet initialised")
        if (not ovly_data.is_multi_group_enabled ()):
            raise RuntimeError ("Multi-group not supported")
        return ovly_data.get_token_from_multi_group_table (index)

    # If ra is a cache address return the corresponding primary storage address,
    # otherwise return ra unchanged.

    def _get_primary_storage_area_ra (self, ra, addr):
        global max_group_size
        orig_ra = ra

        ovly_data = overlay_data.fetch ()
        if (not ovly_data.comrv_initialised ()):
            raise RuntimeError ("ComRV is not initialised")
        mg_index_offset = ovly_data.multi_group_index_offset ()

        cache_start = ovly_data.cache ().start_address ()
        cache_end = ovly_data.cache ().end_address ()
        if (ra >= cache_start and ra < cache_end):
            prev_frame = comrv_stack_frame (addr, mg_index_offset)

            if ((prev_frame.token () & 0x1) != 0x1):
                raise RuntimeError ("returning to overlay function, "
                                    + "second stack frame token is "
                                    + str (prev_frame.token ()))

            token = prev_frame.token ()
            if is_multi_group_token_p (token):
                idx = mg_token_group_id (token)
                token = self._get_multi_group_table_by_index (idx)

            group_id = (token >> 1) & 0xffff
            func_offset = (token >> 17) & 0x3ff
            alignment = prev_frame.align ()
            group_size = ovly_data.group (group_id).size_in_bytes ()
            max_grp_size = max_group_size.value
            group_offset = (func_offset
                            + ((orig_ra - func_offset
                                - alignment) & (max_grp_size - 1)))
            base_addr = ovly_data.group (group_id).base_address ()
            ra = base_addr + group_offset

            debug ("Unwinder:")
            debug ("  frame.return_addr: " + hex (orig_ra))
            debug ("  group_id: " + str (group_id))
            debug ("  func_offset: " + hex (func_offset))
            debug ("  alignment: " + hex (alignment))
            debug ("  group_size: " + hex (group_size))
            debug ("  max_group_size: " + hex (max_grp_size))
            debug ("  group_offset: " + hex (group_offset))
            debug ("  base_addr: " + hex (base_addr))
            debug ("  ra: " + str (ra))
        return ra

    def _unwind (self, addr, allow_uninitialised_frame_p = False):
        """Perform an unwind of one ComRV stack frame.  ADDR is the
        address of a frame on the ComRV stack.  This function returns
        a tuple of the address to return to and the previous ComRV
        stack frame pointer.

         If the stack can't be unwound then an error is thrown."""
        ovly_data = overlay_data.fetch ()
        if (not ovly_data.comrv_initialised ()):
            raise RuntimeError ("ComRV is not initialised")
        mg_index_offset = ovly_data.multi_group_index_offset ()

        # Create a stack frame object at ADDR to represent the stack
        # frame we are unwinding.
        labels = ovly_data.labels ()
        frame = comrv_stack_frame (addr, mg_index_offset)
        assert (labels.ret_from_callee != None)
        while (frame.return_address () == labels.ret_from_callee
               and frame.return_address () != 0
               and frame.offset () != 0xdead):
            addr += frame.offset ()
            frame = comrv_stack_frame (addr, mg_index_offset)

        # Check for an uninitialised stack frame.  We allow this case when
        # performing stack unwinding as we need to be able to unwind at the
        # point when the stack is in the process of being set up.
        if (not allow_uninitialised_frame_p
            and (frame.return_address () == 0
                 and frame.token () == 0)):
            raise RuntimeError ("uninitialized comrv stack frame")

        # Check to see if we have hit the top of the ComRV Stack.
        if (frame.offset () == 0xdead):
            raise RuntimeError ("hit top of ComRV stack (2)")

        # Adjust the ComRV stack pointer; ADDR is now the ComRV stack
        # pointer as it was in the previous frame.
        addr += frame.offset ()

        # Grab the return address from the ComRV stack.  This can be
        # the address of an overlay, or non-overlay function.
        ra = frame.return_address ()

        return self._get_primary_storage_area_ra (ra, addr), addr

    def _unwind_before_comrv_exit (self, pending_frame, labels):
        # Check we're in the valid range of $pc values for this unwinder.
        pc = pending_frame.read_register ("pc").cast (self.void_ptr_t)
        assert (pc > labels.comrv_ret_from_callee_context_switch
                and pc < labels.comrv_exit)

        # This disassembly deliberately starts early so we can figure
        # out the stack adjustment that is required.
        regs = comrv_disassemble_and_analyse (labels.comrv_igonr_caller_thunk_stack_frame,
                                              pc)

        # Now we can calculate the stack pointer value for this frame-id.
        sp = pending_frame.read_register ("sp")
        if (regs["sp"].type == "pv_register" and regs["sp"].reg == "sp"):
            sp = int (sp) - regs["sp"].addend
        frame_pc = gdb.Value (labels.comrv_entry).cast (self.void_ptr_t)
        sp = gdb.Value (sp).cast (self.void_ptr_t)
        unwind_info = pending_frame.create_unwind_info (self.frame_id (sp, frame_pc))

        # Read the current values for $t3 and $ra.
        t3 = pending_frame.read_register ("t3").cast (self.void_ptr_t)
        ra = int (pending_frame.read_register ("ra").cast (self.void_ptr_t))

        # The token or address we're going to retunr too will have
        # been placed into $a0 already if this unwinder is reached,
        # but within the scope of this unwinder the value in $a0 is
        # moved back into $ra.
        if (not (regs["a0"].type == "pv_register" and regs["a0"].reg == "ra")
            and (ra == int (pc))):
            ra = int (pending_frame.read_register ("a0").cast (self.void_ptr_t))

        ovly_data = overlay_data.fetch ()
        if (not ovly_data.comrv_initialised ()):
            raise RuntimeError ("ComRV is not initialised")
        cache_start = ovly_data.cache ().start_address ()
        cache_end = ovly_data.cache ().end_address ()
        if (ra >= cache_start and ra < cache_end):
            ra = self._get_primary_storage_area_ra (ra, int (t3))
        ra = gdb.Value (ra).cast (self.void_ptr_t)

        unwind_info.add_saved_register ("sp", sp)
        unwind_info.add_saved_register ("pc", ra)
        unwind_info.add_saved_register ("t3", t3)
        unwind_info.add_saved_register ("t4", pending_frame.read_register ("t4"))

        return unwind_info

    def _unwind_through_comrv_stack (self, pending_frame, labels):
        # Build the unwind info.  No stack adjustment is needed here,
        # the stack pointer has been restored to the value it had when
        # comrvEntry was called.
        sp = pending_frame.read_register ("sp").cast (self.void_ptr_t)
        frame_pc = gdb.Value (labels.comrv_entry).cast (self.void_ptr_t)
        unwind_info = pending_frame.create_unwind_info (self.frame_id (sp, frame_pc))

        # Find the values of the registers in the caller's frame and
        # save them in the result:
        t3 = pending_frame.read_register ("t3").cast (self.void_ptr_t)
        prev_t4 = t3
        ra, t3 = self._unwind (int (t3))
        unwind_info.add_saved_register("pc", gdb.Value (ra).cast (self.void_ptr_t))
        unwind_info.add_saved_register("t3", gdb.Value (t3).cast (self.void_ptr_t))
        unwind_info.add_saved_register("t4", prev_t4)
        unwind_info.add_saved_register("sp", sp)

        # Return the result:
        return unwind_info

    def _unwind_early_common (self, pending_frame, labels):
        pc = pending_frame.read_register ("pc").cast (self.void_ptr_t)

        # Disassemble from the start of comrvEntry up to the current
        # value of the program counter.  Use this disassembly to
        # analyse the current state of the machine, and apply our
        # understanding of how ComRV operates to this state in order
        # to build a picture of how to unwind from here.

        regs = comrv_disassemble_and_analyse (labels.comrv_entry, pc)

        # Create UnwindInfo.  Usually the frame is identified by the
        # stack pointer and the program counter.  We try to be good
        # and always use the stack pointer as it was at the start of
        # this frame.
        sp = pending_frame.read_register ("sp")
        if (regs["sp"].type == "pv_register"
            and regs["sp"].reg == "sp"):
            sp = int (sp) - regs["sp"].addend

        frame_pc = gdb.Value (labels.comrv_entry).cast (self.void_ptr_t)
        sp = gdb.Value (sp).cast (self.void_ptr_t)
        unwind_info = pending_frame.create_unwind_info (self.frame_id (sp, frame_pc))

        # Setup unwinding for the `s` registers.  This logic is correct in a
        # non-rtos world, but is not going to be correct once we start seeing
        # builds of ComRV for RTOS as these registers get spilt to the stack.
        unwind_info.add_saved_register ("fp", pending_frame.read_register ("fp"))
        unwind_info.add_saved_register ("s1", pending_frame.read_register ("s1"))
        unwind_info.add_saved_register ("s2", pending_frame.read_register ("s2"))
        unwind_info.add_saved_register ("s3", pending_frame.read_register ("s3"))
        unwind_info.add_saved_register ("s4", pending_frame.read_register ("s4"))
        unwind_info.add_saved_register ("s5", pending_frame.read_register ("s5"))
        unwind_info.add_saved_register ("s6", pending_frame.read_register ("s6"))
        unwind_info.add_saved_register ("s7", pending_frame.read_register ("s7"))
        unwind_info.add_saved_register ("s8", pending_frame.read_register ("s8"))
        unwind_info.add_saved_register ("s9", pending_frame.read_register ("s9"))
        unwind_info.add_saved_register ("s10", pending_frame.read_register ("s10"))
        unwind_info.add_saved_register ("s11", pending_frame.read_register ("s11"))

        return unwind_info, regs, pc, sp

    def _unwind_before_context_switch (self, pending_frame, labels):
        unwind_info, regs, pc, sp = self._unwind_early_common (pending_frame,
                                                           labels)

        # This is what we know about the range of possible pc values.
        assert (pc >= labels.comrv_entry
                and pc <= labels.comrv_entry_context_switch)

        # Unwind $t4 which contains the "next" comrv stack frame pointer.
        # Once we've setup the comrv stack frame we're about to use then the
        # old "next" frame, is the frame we're now about to use.
        if (regs["t4"].type == "pv_register"
            and regs["t4"].reg == "t4"):
            unwind_info.add_saved_register("t4", pending_frame.read_register ("t4"))
        elif (regs["t2"].type == "pv_register"
            and regs["t2"].reg == "t4"):
            unwind_info.add_saved_register("t4", pending_frame.read_register ("t2"))
        else:
            t3 = int (pending_frame.read_register ("t3").cast (self.void_ptr_t))
            t3 &= 0xfffffffe
            t3 = gdb.Value (t3).cast (self.void_ptr_t)
            unwind_info.add_saved_register("t4", t3)

        # Unwinding of $t3 is tricky.
        if (regs["t3"].type == "pv_unknown"
            and regs["t2"].type == "pv_register"
            and regs["t2"].reg == "t4"):
            t2 = int (pending_frame.read_register ("t2").cast (self.void_ptr_t))
            t3 = int (pending_frame.read_register ("t3").cast (self.void_ptr_t))
            t3 = gdb.Value (t2 + t3).cast (self.void_ptr_t)
        elif (regs["t3"].type == "pv_register"
              and regs["t3"].reg == "t3"):
            t3 = pending_frame.read_register ("t3")
        elif ((regs["t3"].type == "pv_register"
               and regs["t3"].reg == "t4")
              or (regs["t2"].type == "pv_unknown"
                  and regs["t3"].type == "pv_unknown"
                  and regs["t4"].type == "pv_unknown")):
            t3 = int (pending_frame.read_register ("t3").cast (self.void_ptr_t))
            t3 &= 0xfffffffe
            ra, t3 = self._unwind (t3, True)
            # The return address we just fetched is not valid as the actual
            # return address might not yet have been written to the stack.
            # Set ra to None here, just to ensure this doesn't get used be
            # accident.
            ra = None
            # Pack the t3 integer back into a Value object.
            t3 = gdb.Value (t3).cast (self.void_ptr_t)
        unwind_info.add_saved_register("t3", t3)

        # If the return address is in the cache region then we need to
        # map the cache address to the address within the storage area
        # that this cache region represented.
        ra = int (pending_frame.read_register ("ra"))
        ovly_data = overlay_data.fetch ()
        if (not ovly_data.comrv_initialised ()):
            raise RuntimeError ("ComRV is not initialised")
        cache_start = ovly_data.cache ().start_address ()
        cache_end = ovly_data.cache ().end_address ()
        if (ra >= cache_start and ra < cache_end):
            ra = self._get_primary_storage_area_ra (ra, int (t3))
        ra = gdb.Value (ra).cast (self.void_ptr_t)
        unwind_info.add_saved_register("pc", ra)
        unwind_info.add_saved_register("sp", sp)
        return unwind_info

    def _unwind_before_invoke_callee (self, pending_frame, labels):
        unwind_info, regs, pc, sp = self._unwind_early_common (pending_frame,
                                                           labels)

        # This is what we know about the range of possible pc values.
        assert (pc > labels.comrv_entry_context_switch
                and pc <= labels.comrv_invoke_callee)

        # Find the values of the registers in the caller's frame and save them
        # in the result.  The stack-pointer is super easy as the early unwind
        # logic figured this out for us.  The other two values need to be
        # pulled from the comrv stack.
        unwind_info.add_saved_register("sp", sp)

        t3 = pending_frame.read_register ("t3").cast (self.void_ptr_t)
        prev_t4 = t3
        ra, t3 = self._unwind (int (t3))

        unwind_info.add_saved_register("pc", gdb.Value (ra).cast (self.void_ptr_t))
        unwind_info.add_saved_register("t3", gdb.Value (t3).cast (self.void_ptr_t))
        unwind_info.add_saved_register("t4", prev_t4)
        return unwind_info

    def _unwind_before_ret_from_callee_context_switch (self, pending_frame, labels):
        # This is what we know about the range of possible pc values.
        pc = int (pending_frame.read_register ("pc").cast (self.void_ptr_t))
        assert (pc >= labels.comrv_igonr_caller_thunk_stack_frame
                and pc <= labels.comrv_ret_from_callee_context_switch)

        # First thing we need to do is figure out the stack pointer at entry
        # to this function.
        #
        # For the region we're looking at the stack pointer does get adjusted,
        # but at the start of the block the stack pointer should be correct.
        regs = comrv_disassemble_and_analyse (labels.comrv_igonr_caller_thunk_stack_frame,
                                              pc)

        # Check we understand the current contents of the stack pointer.
        if (regs["sp"].type != "pv_register" or regs["sp"].reg != "sp"):
            return None

        sp = int (pending_frame.read_register ("sp").cast (self.void_ptr_t))
        sp = sp - regs["sp"].addend
        sp = gdb.Value (sp).cast (self.void_ptr_t)
        frame_pc = gdb.Value (labels.comrv_entry).cast (self.void_ptr_t)
        unwind_info = pending_frame.create_unwind_info (self.frame_id (sp, frame_pc))

        unwind_info.add_saved_register("sp", sp)

        if (regs["t3"].type == "pv_register" and regs["t3"].reg == "t5"):
            prev_t3 = pending_frame.read_register ("t3")
        else:
            prev_t3 = pending_frame.read_register ("t5")
        unwind_info.add_saved_register("t3", prev_t3)

        # If the return address is in the cache region then we need to
        # map the cache address to the address within the storage area
        # that this cache region represented.
        ra = int (pending_frame.read_register ("ra"))
        ovly_data = overlay_data.fetch ()
        if (not ovly_data.comrv_initialised ()):
            raise RuntimeError ("ComRV is not initialised")
        cache_start = ovly_data.cache ().start_address ()
        cache_end = ovly_data.cache ().end_address ()
        if (ra >= cache_start and ra < cache_end):
            ra = self._get_primary_storage_area_ra (ra, int (prev_t3))
        ra = gdb.Value (ra).cast (self.void_ptr_t)
        unwind_info.add_saved_register("pc", ra)

        if (regs["t4"].type == "pv_register" and regs["t4"].reg == "t3"):
            unwind_info.add_saved_register("t4", pending_frame.read_register ("t4"))
        else:
            unwind_info.add_saved_register("t4", pending_frame.read_register ("t3"))

        return unwind_info

    def _unwind_at_return (self, pending_frame, labels):
        # Check where we are.
        pc = int (pending_frame.read_register ("pc").cast (self.void_ptr_t))
        assert (pc == labels.comrv_exit)

        # We are at the return point.  All registers that are going to be
        # restored should have been restored by now.
        frame_pc = gdb.Value (labels.comrv_entry).cast (self.void_ptr_t)
        sp = pending_frame.read_register ("sp").cast (self.void_ptr_t)
        unwind_info = pending_frame.create_unwind_info (self.frame_id (sp, frame_pc))
        unwind_info.add_saved_register("sp", sp)
        unwind_info.add_saved_register("pc", pending_frame.read_register ("ra"))
        unwind_info.add_saved_register("t3", pending_frame.read_register ("t3"))
        unwind_info.add_saved_register("t4", pending_frame.read_register ("t4"))
        return unwind_info

    def _unwind_before_comrv_end (self, pending_frame, labels):
        # Check where we are.
        pc = int (pending_frame.read_register ("pc").cast (self.void_ptr_t))
        assert (pc > labels.comrv_exit and pc < labels.comrv_end)

        # Build the unwind info.
        sp = pending_frame.read_register ("sp").cast (self.void_ptr_t)
        frame_pc = gdb.Value (labels.comrv_entry).cast (self.void_ptr_t)
        unwind_info = pending_frame.create_unwind_info (self.frame_id (sp, frame_pc))

        prev_t3 = pending_frame.read_register ("t3").cast (self.void_ptr_t)

        # If the return address is in the cache region then we need to
        # map the cache address to the address within the storage area
        # that this cache region represented.
        ra = int (pending_frame.read_register ("ra"))
        ovly_data = overlay_data.fetch ()
        if (not ovly_data.comrv_initialised ()):
            raise RuntimeError ("ComRV is not initialised")
        cache_start = ovly_data.cache ().start_address ()
        cache_end = ovly_data.cache ().end_address ()
        if (ra >= cache_start and ra < cache_end):
            ra = self._get_primary_storage_area_ra (ra, int (prev_t3))
        ra = gdb.Value (ra).cast (self.void_ptr_t)
        unwind_info.add_saved_register("pc", ra)

        #t3 = pending_frame.read_register ("t3").cast (self.void_ptr_t)
        t4 = pending_frame.read_register ("t4").cast (self.void_ptr_t)
        unwind_info.add_saved_register("t3", prev_t3)
        unwind_info.add_saved_register("t4", t4)
        unwind_info.add_saved_register("sp", sp)
        return unwind_info

    def __call__ (self, pending_frame):
        # Check if we are inside the core ComRV function that runs
        # from the comrv entry label to the comrv exit label.
        labels = overlay_data.fetch ().labels ()
        # Lookup void pointer type again in case setting an executable changed
        # it.  If void_ptr_t has the wrong length, it will cause an invalid cast
        # error.
        self.void_ptr_t = gdb.lookup_type("void").pointer()
        pc = pending_frame.read_register ("pc").cast (self.void_ptr_t)
        if (not labels.enabled
            or pc < labels.comrv_entry or pc >= labels.comrv_end):
            return None

        # We know that the user is unside the comrv entry point.  At this
        # point we are allowed to assume that comrv is initialised.  If comrv
        # is not initialised then the user has messed up (calling into comrv
        # before initialisation) and so all bets are off.
        global_mark_comrv_as_initialised ()

        # For unwinding we split the ComRV assembler core into
        # regions, and use a different unwinder for each region.  When
        # working on this unwinder please consider that the frame_id
        # created in each different unwinder should use the $pc value
        # for 'comrvEntry' and should use the $sp value as it was at
        # the point of entry into 'comrvEntry'; this ensures that the
        # frame-id will not change as the user single steps through
        # the assembler core, a changing frame-id will confuse GDB.
        #
        # Each unwind handler should start with an assertion for the
        # range of $pc values that it handles.
        if (pc <= labels.comrv_entry_context_switch):
            return self._unwind_before_context_switch (pending_frame, labels)
        elif (pc <= labels.comrv_invoke_callee):
            return self._unwind_before_invoke_callee (pending_frame, labels)
        elif (pc < labels.comrv_igonr_caller_thunk_stack_frame):
            return self._unwind_through_comrv_stack (pending_frame, labels)
        elif (pc <= labels.comrv_ret_from_callee_context_switch):
            return self._unwind_before_ret_from_callee_context_switch (pending_frame, labels)
        elif (pc < labels.comrv_exit):
            return self._unwind_before_comrv_exit (pending_frame, labels)
        elif (pc == labels.comrv_exit):
            return self._unwind_at_return (pending_frame, labels)
        elif (pc < labels.comrv_end):
            return self._unwind_before_comrv_end (pending_frame, labels)

        raise RuntimeError ("no unwinder logic for address 0x%x" % pc)

# Register the ComRV stack unwinder.
gdb.unwinder.register_unwinder (None, comrv_unwinder (), True)

#=====================================================================#
#                         Final Setup
#=====================================================================#
#
# Perform some final initialisation steps.
#
#=====================================================================#

# Create an instance of the command class.
ParseComRV ()

comrv_prefix_command ()
comrv_status_command ()
comrv_stack_command ()
comrv_groups_command ()

# Create an instance of the overlay manager class.
MyOverlayManager ()

gdb.execute ("overlay auto", False, False)


# This line is commented out, but left in at the request of WD.
# Turning this packet off will force GDB to make use of read/write
# software breakpoints, however, on some targets these don't appear to
# play well, probably with pipeline caching or some such.
#
# gdb.execute ("set remote software-breakpoint-packet off", False, False)

