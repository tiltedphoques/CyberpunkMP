import ida_kernwin
import idaapi

# Point to the first function address in the vftable
ea = ida_kernwin.get_screen_ea()
vftable_size = 0

while True:
    address = idaapi.get_qword(ea)
    
    if not idaapi.is_code(idaapi.get_flags(address)):
        break
    
    vftable_size = vftable_size + 1
    if vftable_size > 1000:
        break
    
    ea = ea + 8

print(vftable_size)