import re
import os
import sys

def writeFile(outf,pluginlist):
    outfile = open(outf, "w")
    outfile.write("#ifndef __PLUGINIDS_H__\n")
    outfile.write("#define __PLUGINIDS_H__\n\n")
    generateIds(outfile, pluginlist)
    generateValueIds(outfile, pluginlist)
    generateValueIdsString(outfile,pluginlist)
    generateDebugFunction(outfile, pluginlist)
    outfile.write("\n#endif /* __PLUGINIDS_H__ */\n")
    outfile.close
    
def generateIds(f,plist):
    f.write("enum PluginIds {\n")
    for p in plist:
        f.write("    Plugin"+p['pname'].capitalize()+" = "+p['pid']+",\n")
    f.write("};\n")
    
def generateValueIds(f,plist):
    for p in plist:
        f.write("enum Plugin"+p['pname'].capitalize()+"Ids {\n")
        for v in p['enumvalue']:
            f.write("    "+v.replace(" ","")+",\n")
        f.write("};\n")

def generateValueIdsString(f,plist):
    for p in plist:
        f.write("const char Plugin"+p['pname'].capitalize()+"IdString[] PROGMEM = \""+p['pname'].capitalize()+"\";\n")
        for v in p['enumvalue']:
            enumval = v.replace(" ","")
            f.write("const char Plugin"+p['pname'].capitalize()+enumval+"IdString[] PROGMEM = \""+enumval+"\";\n")
    f.write("const char PluginUnknown[] PROGMEM = \"unknown\";\n")

def generateDebugFunction(f,plist):
    f.write("const char* const getPluginNameDebug(int pid) {\n\tswitch(pid){\n")
    for p in plist:
        f.write("\t\tcase "+p['pid']+": return Plugin"+p['pname'].capitalize()+"IdString;\n")
    f.write("\t\tdefault: return PluginUnknown;\n")
    f.write("}\n}\n")
    f.write("const char* const getPluginValueNameDebug(int pid, int vid) {\n\tswitch(pid){\n")
    for p in plist:
        f.write("\t\tcase "+p['pid']+": switch(vid){\t\t\t\n")
        i = 0
        for v in p['enumvalue']:
            f.write("\t\t\t\tcase "+str(i)+": return Plugin"+p['pname'].capitalize()+v.replace(" ","")+"IdString;\n")
            i+=1
        f.write("\t\t\t\tdefault: return PluginUnknown;\n")
        f.write("}\n")
    f.write("\t\t\tdefault: return PluginUnknown;\n")
    f.write("}\n}\n")
    f.write("#ifdef NDEBUG\n")
    f.write("#define DBGPRINTMESSAGELN(level,message)\n")
    f.write("#else\n")
    f.write("#define DBGPRINTMESSAGELN(level,message) ({\\\n")
    f.write("       char msgbuffer[64];\\\n")
    f.write("         snprintf(msgbuffer,sizeof(msgbuffer),\"%s \",getPluginNameDebug(message->getSenderId()));\\\n")
    f.write("       DPRINTLN(level,msgbuffer);\\\n")
    f.write("        if(message->hasData()) {\\\n")
    f.write("    for(unsigned int index = 0 ; index < message->getEntryCount(); index++) {\\\n")
    f.write("        if(message->isType<BoolValue>(index))\\\n")
    f.write("            snprintf(msgbuffer,sizeof(msgbuffer),\"%s-%s: bool %d\",getPluginNameDebug(message->getSenderId()),getPluginValueNameDebug(message->getSenderId(),message->get(index).getId()),message->getAs<BoolValue>(index).value);\\\n")
    f.write("        else if(message->isType<FloatValue>(index))\\\n")
    f.write("            snprintf(msgbuffer,sizeof(msgbuffer),\"%s-%s: float %f\",getPluginNameDebug(message->getSenderId()),getPluginValueNameDebug(message->getSenderId(),message->get(index).getId()),message->getAs<FloatValue>(index).value);\\\n")
    f.write("        else if(message->isType<IntValue>(index))\\\n")
    f.write("            snprintf(msgbuffer,sizeof(msgbuffer),\"%s-%s: int %d\",getPluginNameDebug(message->getSenderId()),getPluginValueNameDebug(message->getSenderId(),message->get(index).getId()),message->getAs<IntValue>(index).value);\\\n")
    f.write("        else if(message->isType<StringValue>(index))\\\n")
    f.write("            snprintf(msgbuffer,sizeof(msgbuffer),\"%s-%s: string %s\",getPluginNameDebug(message->getSenderId()),getPluginValueNameDebug(message->getSenderId(),message->get(index).getId()),message->getAs<StringValue>(index).value.c_str());\\\n")
    f.write("        else\\\n")
    f.write("            snprintf(msgbuffer,sizeof(msgbuffer),\"%s-%s: unknown\",getPluginNameDebug(message->getSenderId()),getPluginValueNameDebug(message->getSenderId(),message->get(index).getId()));\\\n")
    f.write("       DPRINTLN(level,msgbuffer);\\\n")
    f.write("    }\\\n")
    f.write("    }\\\n")
    f.write("  })\n")
    f.write("#endif\n")
        
def parseFile(fin):
    f = open(fin, "r")
    data = f.read().replace('\n', '')
    #print(data)
    x = re.search(r'(enum)\s+(\w+)\s+\{\s*([^\}]+)\s*\};(.*Plugin\s*\((\d*)\s*,\s*"([^"]*)")', data)
    if x:
        return x.group(5), x.group(6), x.group(3)
    else:
        return None,None,None

try:
    scriptdir = os.path.dirname(os.path.abspath(__file__))
except NameError:
    approot = os.path.dirname(os.path.abspath(sys.argv[1]))
    scriptdir = os.path.join(approot, 'thirdparty')
genfilename = 'pluginids.h'  
gendir = scriptdir+'/'+genfilename                 
print(scriptdir)

files = os.listdir(scriptdir)
files = [scriptdir+'/'+f for f in files if os.path.isfile(scriptdir+'/'+f) & f.endswith('h')] 
print(*files, sep="\n")
pluginlist = list()
for f in files:
    pid, pname,enumvalue = parseFile(f)
    if pid != None:
        print(pid+":"+pname+":"+enumvalue)
        vids = enumvalue.split(",")
        pluginlist.append({'pid':pid,'pname':pname,'enumvalue':list(vids)})
writeFile(gendir,pluginlist)
