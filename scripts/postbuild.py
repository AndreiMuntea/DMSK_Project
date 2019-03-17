import os
import sys 
import shutil
import subprocess

def Copy(Source, Destination):
    if os.path.isfile(Destination):
        os.unlink(Destination)
    
    print("Copying from", Source, "to", Destination)
    shutil.copyfile(Source, Destination)

def ClearFolder(Folder):
    if os.path.isdir(Folder):
        shutil.rmtree(Folder)
    
    if not os.path.isdir(Folder):
        os.mkdir(Folder)

def CreateDirs(DirectoryPath):
    if not os.path.isdir(DirectoryPath):
        os.makedirs(DirectoryPath)

def CopyProjectFiles(Platform, Configuration, OutDir, ProjectName, BinPath):
    project_pdb = ProjectName + '.pdb'
    project_out = ProjectName + '.exe'
    
    if ProjectName in ['IOCTLFirstDriver']:
        project_out = ProjectName + '.sys'

    pdb_src = os.path.join(OutDir, project_pdb)
    pdb_dst = os.path.join(BinPath, "bin", ProjectName, Platform, Configuration, project_pdb)

    out_src = os.path.join(OutDir, project_out)
    out_dst = os.path.join(BinPath, "bin", ProjectName, Platform, Configuration, project_out)

    CreateDirs(os.path.dirname(pdb_dst))
    CreateDirs(os.path.dirname(out_dst))

    Copy(pdb_src, pdb_dst)
    Copy(out_src, out_dst)
    
def GenerateTmfFiles(Platform, Configuration, OutDir, ProjectName, BinPath):
    pdb_path = os.path.join(BinPath, "bin", ProjectName, Platform, Configuration)
    tmf_path = os.path.join(BinPath, "tmf", ProjectName, Platform, Configuration)

    CreateDirs(tmf_path)
    ClearFolder(tmf_path)

    args = ['tracepdb.exe', '-f',  pdb_path + '\\', '-p', tmf_path + '\\', '-c', '-v']
    tracepdb_args = ' '.join(args)
    print("Tracepdb arguments: ", tracepdb_args)

    tracepdb_proc = subprocess.Popen(tracepdb_args, stdout=subprocess.PIPE)
    tracepdb_proc.wait()

    print("Tracepdb exit code : ", tracepdb_proc.returncode)

def Process(Platform, Configuration, OutDir, ProjectName, BinPath):
    CopyProjectFiles(Platform, Configuration, OutDir, ProjectName, BinPath)
    GenerateTmfFiles(Platform, Configuration, OutDir, ProjectName, BinPath)


if __name__ == "__main__":
    if (len(sys.argv) < 6):
        print("Usage: postbuild.py <platform> <configuration> <output directory> <project name> <bin path>")
        exit(1)

    Process(*sys.argv[1:])
    exit(0)