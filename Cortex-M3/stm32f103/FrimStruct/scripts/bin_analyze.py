#!/usr/bin/env python

import os
import sys
import logging

import nose
import cle
import angr

import claripy
import struct

import matplotlib, matplotlib.pyplot as plt
import pygraphviz
import networkx as nx
import numpy as np
import pylab


from angr import BP_AFTER, BP_BEFORE


'''
The memchnism to analyze a binary file:
    1) Locate the vector table and the addresses of the handlers
    2) Locate the main function in the binary by loop detection
        2.1) Identify the code portion in main for intializing the execution of app code
            2.1.1) the instruction used to initialize the CSTACK for the app code
            2.1.2) the instruction used to jump to the app code (out of the boot code)
        2.2) Identify the code portion in main for ISP programing
            2.2.1) the function (subroutine) for write data to flash
    3) Locate the data source and date sink namely the function of USART_TX and USART_RX:
        3.1) Identify the memory buffer for storing the received data
        3.2) Identify the memory buffer for storing the data to be sent
'''


INSN_CPSIE = 0xB663 # Enable  interrupt
INSN_CPSID = 0xB672 # Disable interrupt
MSR_ADDR   = 0x80017AC

def drawGraph(cfg, name):
    #A = nx.to_agraph(cfg)
    #A.draw(name+".pdf", format="pdf", prog="dot")
    nx.draw(cfg, with_labels=True)
    plt.draw()
    plt.show()

def unpackVectorTable(binFile, offset=0):
    if os.path.isfile(binFile):
        table_size = 59
        vector = []
        raw = open(binFile, 'rb').read()
        for i in range(table_size):
            off = i * 4 + offset
            addr = struct.unpack('<I', raw[off:off+4])[0]
            #print "%02d 0x%04x 0x%08x" % (i, off, addr)
            vector.append(addr)
    return vector

def cfgAnalysis(binFile, bin_base, bin_entry, cstack):
    print("CFG Analysis: bin_base=0x%08x, bin_entry=0x%08x" % (bin_base, bin_entry))

    vector_table = unpackVectorTable(binFile)

    proj = angr.Project(binFile,
            #default_analysis_mode='symbolic',
            load_options={
                'main_opts': {
                    'backend': 'blob',
                    'custom_base_addr': bin_base,
                    'custom_entry_point': bin_entry,
                    'custom_arch': 'ARM',
                    'custom_offset': 0,
                    },
                }
            )
    '''
    a_cfg = proj.analyses.CFG(regions=[(0x80000EC, 0x8001FFF)],
            collect_data_references=True,
            extra_cross_references=True,
            show_progressbar=True, 
            switch_mode_on_nodecode=True)
    '''
    a_cfg = proj.analyses.CFGAccurate(
            show_progressbar=True,
            context_sensitivity_level=1,
            starts=vector_table[1:],
            keep_state=True, 
            enable_advanced_backward_slicing=True,
            #enable_symbolic_back_traversal=True,
            normalize=False,
            fail_fast=True)
    # functions in function manager
    #for faddr, f in proj.kb.functions.items():
    #    print f

    fun_cfg = a_cfg.functions.callgraph
    drawGraph(fun_cfg, "fun")
    reset_handler = a_cfg.functions.function(addr=vector_table[1])
    print(reset_handler)
    succs = fun_cfg.successors(reset_handler.addr)
    while len(succs) > 0:
        for succ in succs:
            succ_function = a_cfg.functions[succ]
            print(succ_function)

'''
Identiry BLE data receive function
'''
def identifyUSARTRecvData():
    return

'''
Identiry BLE data send function
'''
def identifyUSARTSendData():
    return

'''
Identiry FLASH write function
'''
def identiryFLASHWrite():
    return

def detectLoop(cfg):
    for node in cfg.nodes():
        print(node)
        print(cfg.in_degree(node))





def explorePath(binFile, bin_base, bin_entry, cstack):
    print("Explore Path: bin_base=0x%08x, bin_entry=0x%08x" % (bin_base, bin_entry))
    proj = angr.Project(binFile, 
            load_options={
                'main_opts': {
                    'backend': 'blob',
                    'custom_base_addr': bin_base,
                    'custom_entry_point': bin_entry,
                    'custom_arch': 'ARM',
                    'custom_offset': 0,
                    },
                }
            )
    state = proj.factory.entry_state()
    irsb_dict = {}
    isMain = False
    '''
    Hook the special instructions, such as: CPSID, CPSIE, MSR
    '''
    def hook_CPSID(state):
        print("CPSID: Disable interrupt")
    def hook_CPSIE(state):
        print("CPSIE: Enable interrupt")
        isMain = True
    def hook_GPIO_ResetBits(state):
        print("GPIO_Reset_bits")
    def hook_Get_Systick_passed(state):
        print("Get_Systick_Passed: ", state.regs.r0)
   
    proj.hook(0x8001743, hook_CPSID, 8)
    proj.hook(0x8001749, hook_CPSIE, 2)
    proj.hook(0x800174F, hook_GPIO_ResetBits, 8)
    proj.hook(0x8001761, hook_Get_Systick_passed, 0)

    def act_add_constraints(state):
        print("Constraints:")
        print(state.inspect.added_constraints)
        succs = state.inspect.sim_successors
        succ_addr = [hex(s.addr) for s in succs]
        print('Added constraints at:', hex(state.addr))
        print('Successors:', succ_addr)
        print("")
    def act_mem_read(state):
        print("Mem Read: ", state.inspect.mem_read_address)
    def act_irsb(state):
        #print "New IRSB: ", hex(state.inspect.address)
        if not state.inspect.address in irsb_dict.keys():
            irsb_dict[state.inspect.address] = 0
        irsb_dict[state.inspect.address] += 1
        #if irsb_dict[state.inspect.address] & 0xfffffffe == 0x800175e:
        print("IRSB: 0x%08x %d" % (state.inspect.address, irsb_dict[state.inspect.address]))
    #state.inspect.b('mem_read', when=BP_AFTER, action=act_mem_read)
    state.inspect.b('irsb', when=BP_AFTER, action=act_irsb)
    #state.inspect.b('constraints', when=BP_AFTER, action=act_add_constraints)

    simgr = proj.factory.simgr(state)
    
    '''
    Initialize the exploration technique
    '''
    def goal_reached_callback(goal, p, pg):
        print("Reached goal callback:")
        print(goal)
        print(p)
        print(pg)

    dm = angr.exploration_techniques.Director(cfg_keep_states=True,
                                                goal_satisfied_callback=goal_reached_callback,
                                                num_fallback_states=1)
    goal = angr.exploration_techniques.ExecuteAddressGoal(MSR_ADDR)
    dm.add_goal(goal)
    simgr.use_technique(dm)

    '''
    ll = angr.exploration_techniques.LoopLimiter()
    simgr.use_technique(ll)
    '''
    
    simgr.explore(find=MSR_ADDR, num_find=1)
    #simgr.run()
    print(simgr.active)

def symbolicExe(binFile, bin_base, bin_entry, cstack):
    print("SymbolicExe: bin_base=0x%08x, bin_entry=0x%08x" % (bin_base, bin_entry))
    proj = angr.Project(binFile, 
            load_options={
                'main_opts': {
                    'backend': 'blob',
                    'custom_base_addr': bin_base,
                    'custom_entry_point': bin_entry,
                    'custom_arch': 'ARM',
                    'custom_offset': 0,
                    },
                }
            )

    state = proj.factory.entry_state()

    '''
    Hook the special instructions, such as: CPSID, CPSIE, MSR
    '''
    def hook_CPSID(state):
        print("CPSID: Disable interrupt")
    def hook_CPSIE(state):
        print("CPSIE: Enable interrupt")
    def hook_MSR(state):
        print("MSR: Chande the address of CSTACK")
    proj.hook(0x8001743, hook_CPSID, 2)
    proj.hook(0x8001749, hook_CPSIE, 2)
    proj.hook(MSR_ADDR, hook_MSR, 4)
    #simgr.run()
    #print simgr
    #return

    '''
    Inspect specific actions, such as mem_read, mem_write
    '''
    def ack_mem_read(state):
        print("Mem Read:")
        print(state.inspect.mem_read_address)
        print(state.inspect.mem_read_expr)
        print(state.inspect.mem_read_length)

    def act_mem_write(state):
        print("Mem Write:")
        print(state.inspect.mem_write_address)
        print(state.inspect.mem_write_expr)
        print(state.inspect.mem_write_length)

    def act_call(state):
        print("Function Call: 0x%08x")
        print(state.inspect.function_address.uint32_t)

    def act_exit(state):
        print("Exit:")
        print(state.inspect.exit_target.concrete)
        print(state.inspect.exit_guard)
        print(state.inspect.exit_jumpkind)
        print(state.inspect.backtrace)

    def act_fork(state):
        succs = state.inspect.sim_successors
        succ_addr = [hex(s.addr) for s in succs]
        print('Fork after:', hex(state.addr))
        print('Successors:', succ_addr)

    def act_add_constraints(state):
        print("Constraints:")
        print(state.inspect.added_constraints)
        succs = state.inspect.sim_successors
        succ_addr = [hex(s.addr) for s in succs]
        print('Added constraints at:', hex(state.addr))
        print('Successors:', succ_addr)
        print("")


    def act_address_concretization(state):
        print("Address Concretization:")
        print(state.inspect.address_concretization_strategy)
        print(state.inspect.address_concretization_action)
        print(state.inspect.address_concretization_memory)
        print(state.inspect.address_concretization_expr)
        print(state.inspect.address_concretization_result)
        print(state.inspect.address_concretization_add_constraints)

    #state.inspect.b('mem_read', when=BP_AFTER, action=ack_mem_read)
    #state.inspect.b('constraints', when=BP_BEFORE, action=act_add_constraints)
    #state.inspect.b('constraints', when=BP_AFTER, action=act_add_constraints)
    #state.inspect.b('exit', when=BP_BEFORE, action=act_exit)
    state.inspect.b('call', when=BP_BEFORE, action=act_call)
    #state.inspect.b('address_concretization', when=BP_BEFORE, action=act_address_concretization)
    #state.inspect.b('fork', when=BP_AFTER, action=act_fork)

    simgr = proj.factory.simgr(state)
    print(simgr.active)


    while True:
        simgr.step(until=lambda lpg: len(lpg.active) > 1)
        succ = simgr.active
        #n_size = len(succ.successors)
        n_size = len(succ)
        if n_size > 1:
            break

    print("Addr: ", hex(succ[0].addr), hex(succ[1].addr))
    state = succ[0]
    print(state.regs.r15, state.regs.r0, state.regs.r1, state.regs.r2, state.regs.r3, state.regs.r4)
    state = succ[1]
    print(state.regs.r15, state.regs.r0, state.regs.r1, state.regs.r2, state.regs.r3, state.regs.r4)



def lookupMainLoop(binFile, bin_base, bin_entry, cstack):
    print("MainLoop: bin_base=0x%08x, bin_entry=0x%08x" % (bin_base, bin_entry))
    
    proj = angr.Project(binFile, 
            load_options={
                'main_opts': {
                    'backend': 'blob',
                    'custom_base_addr': bin_base,
                    'custom_entry_point': bin_entry,
                    'custom_arch': 'ARM',
                    'custom_offset': 0,
                    },
                }
            )
    state = proj.factory.entry_state()
    state.regs.r0 = cstack
    state = proj.factory.entry_state()
    print(state.regs.r0)
    a_cfg =  cfg = proj.analyses.CFGAccurate(
            context_sensitivity_level=1,
            #start=state,
            keep_state=True,
            enable_advanced_backward_slicing=True
            #enable_symbolic_back_traversal=True
            )
    print(a_cfg)
    for k in a_cfg.kb.functions:
        print("0x%08x" % k)
        entry_func = a_cfg.kb.functions[k]
        print(a_cfg.kb.functions[k])
        print("String references:")
        print(entry_func.string_references)
        print("Runtime values:")
        #print entry_func.runtime_values
        print("Code constants:")
        for t2 in entry_func.code_constants:
            print("\t0x%08x" % t2)
        print("Call targets:")
        for t3 in entry_func.get_call_sites():
            print("\t0x%08x" % entry_func.get_call_target(t3))
    print("Deadends:")
    print(cfg.deadends)
    print("")
    
    a_cfg_graph = a_cfg.graph
    print(type(a_cfg_graph))
    detectLoop(a_cfg_graph)
    drawGraph(a_cfg_graph, "test")
    #loop_back_edges = a_cfg.get_loop_back_edges()
    #print loop_back_edges


    #cdg = proj.analysis.CDG(cfg=cfg)

    '''
    fast_cfg = proj.analyses.CFGFast(show_progressbar=True, switch_mode_on_nodecode=True)
    for k in fast_cfg.kb.functions:
        entry_func = fast_cfg.kb.functions[k]
        print fast_cfg.kb.functions[k]
        print entry_func.string_references()
    '''


def analyzeBoot(binFile):
    (cstack, reset_handler) = unpackVectorTable(bin_file, 0)[0:2]
    bin_base  = reset_handler & 0xFFFF2000
    #bin_entry     = reset_handler & 0xFFFFFFFE
    #bin_entry = reset_handler
    bin_entry = 0x8001741 # main 
    #lookupMainLoop(binFile, bin_base, bin_entry, cstack)
    #symbolicExe(binFile, bin_base, bin_entry, cstack)
    #explorePath(binFile, bin_base, bin_entry, cstack)
    cfgAnalysis(binFile, bin_base, bin_entry, cstack)

if __name__ == "__main__":
    bin_file = sys.argv[1]
    analyzeBoot(bin_file)
