import re
import os
import sys

def writeFile(outf,pluginlist):
    outfile = open(outf, "w")
    outfile.write("#ifndef __PLUGINIDS_H__\n")
    outfile.write("#define __PLUGINIDS_H__\n\n")
    generateIds(outfile, pluginlist)
    generateValueIds(outfile, pluginlist)
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

def gggenerateIds(f,pid,pname,vid):
    f.write("const int "+pname+"PluginId = "+pid+";\n")
    index = 0
    for vi in vids:
        vi = vi.replace(" ","")
        f.write("const int "+pname+"PluginValueId_"+vi+" = "+str(index)+";\n")
        index+=1
        #print(id)
        
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
    scriptdir= os.path.join(approot, 'thirdparty')
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
