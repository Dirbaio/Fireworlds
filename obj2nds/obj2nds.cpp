#include<iostream>
#include<fstream>
#include<cmath>
#include<vector>
#include<sstream>
#include<cstdlib>
#include<iomanip>
#include "fifocommands.h"

using namespace std;

class vec3 {
    public:
        float x, y, z;
        vec3(){x=0; y=0; z=0;}
        vec3(float X, float Y, float Z){
            x=X;
            y=Y;
            z=Z;
        }
        vec3 operator+ (vec3  b){return vec3(x+b.x, y+b.y, z+b.z);}
        vec3 operator- (vec3  b){return vec3(x-b.x, y-b.y, z-b.z);}
        vec3 operator* (float b){return vec3(x*b, y*b, z*b);}
        vec3 operator/ (float b){return vec3(x/b, y/b, z/b);}
        float length(){return sqrt(x*x + y*y + z*z);}
        void print(){cout << "vec3(" << x << ", " << y << ", " << z << ")" << endl;}
};
 
vec3 normalize(vec3 a){return a / a.length();}
float dotproduct(vec3 a, vec3 b){return a.x*b.x + a.y*b.y + a.z*b.z;}
vec3 crossproduct(vec3 a, vec3 b){return vec3(a.y*b.z-a.z*b.y, a.z*b.x-a.x*b.z, a.x*b.y-a.y*b.x);}


struct tex
{
	float u, v, w;
};

vector<vec3> vtxs(0);
vector<vec3> texs(0);
vector<vec3> norms(0);

int listSize;
int quadCount = 0;
int triCount = 0;
float scale = 1;
float texScale = 1;
bool recalcNormals = false;
bool writeNormals = true;
bool writeTexCoords = true;

bool asmoutput = false;

int str2i(const string& s)
{
	stringstream ss(s);
	int r;
	ss>>r;
	return r;
}


vector<int> result(0);

void writeInt(int c)
{
	result.push_back(c);
	listSize++;
}


void writeColorCmd(int red, int green, int blue)
{
	writeInt(FIFO_COLOR);
	writeInt(RGB15(red>>3, green>>3, blue>>3));
}

void writeVtxCmd(vec3& v)
{
	writeInt(FIFO_VERTEX16);
	writeInt(VERTEX_PACK(floattov16(v.x*scale), floattov16(v.y*scale)));
	writeInt(VERTEX_PACK(floattov16(v.z*scale), 0));
}

void writeNormCmd(vec3& n)
{
	writeInt(FIFO_NORMAL);
	writeInt(NORMAL_PACK(floattov10(n.x), floattov10(n.y), floattov10(n.z)));
}

void writeTexCmd(vec3& t)
{
	writeInt(FIFO_TEX_COORD);
	writeInt(TEXTURE_PACK(floattot16(t.x), floattot16(t.y)));
	
}
void parseVtx(string abc, vec3& vv, vec3& tt, vec3& nn)
{
	stringstream ss(abc);
	
	string a, b, c;
	
	getline(ss, a, '/');
	getline(ss, b, '/');
	getline(ss, c, '/');
	
	
	int v = str2i(a)-1;
	int t = str2i(b)-1;
	int n = str2i(c)-1;
	if(n >= norms.size()) cout<<"ERROR: out of bounds normal num"<<endl;
	else nn = norms[n];
	if(v >= vtxs.size()) cout<<"ERROR: out of bounds vtx num"<<endl;
	else vv = vtxs[v];
	if(t >= texs.size()) cout<<"ERROR: out of bounds tex num"<<endl;
	else tt = texs[t];
}


void writeFace(stringstream& ss, int& oldMode)
{
	string a, b, c, d;
	ss>>a>>b>>c>>d;
	
	vec3 va, vb, vc, vd;
	vec3 na, nb, nc, nd;
	vec3 ta, tb, tc, td;
	parseVtx(a, va, ta, na);
	parseVtx(b, vb, tb, nb);
	parseVtx(c, vc, tc, nc);
	int mode = GL_TRIANGLES;
	if(ss)
	{
		mode = GL_QUADS;
		parseVtx(d, vd, td, nd);
	}
	
	if(mode != oldMode)
	{
		writeInt(FIFO_BEGIN);
		writeInt(mode);
		oldMode = mode;
	}
	
	if(recalcNormals && writeNormals)
	{
		
		vec3 v1 = vb - va;
		vec3 v2 = vc - va;
		vec3 normal = crossproduct(v1, v2);
		normal = normalize(normal);
		writeNormCmd(normal);
		
	}
	
	if(!recalcNormals && writeNormals) writeNormCmd(na);
	if(writeTexCoords) writeTexCmd(ta);
	writeVtxCmd(va);
	if(!recalcNormals && writeNormals) writeNormCmd(nb);
	if(writeTexCoords) writeTexCmd(tb);
	writeVtxCmd(vb);
	if(!recalcNormals && writeNormals) writeNormCmd(nc);
	if(writeTexCoords) writeTexCmd(tc);
	writeVtxCmd(vc);
	if(mode == GL_QUADS)
	{
		if(!recalcNormals && writeNormals) writeNormCmd(nd);
		if(writeTexCoords) writeTexCmd(td);
		writeVtxCmd(vd);
	}
	
	if(mode == GL_QUADS)
		quadCount++;
	else
		triCount++;

}

bool parseBool(string s)
{
	if(s == "yes") return true;
	if(s == "no") return false;
	cerr<<"Not a bool value: "<<s<<endl;
	exit(1);
}

void writeBinaryOutput(string file)
{
	ofstream out (file.c_str(), ios_base::out | ios_base::binary);
	
	if(!out)
	{
		cout<<"Fail opening output file."<<endl;
		exit(1);
	}
	
	for(int i = 0; i < result.size(); i++)
	{
		int c = result[i];
		out.put(c>>0);
		out.put(c>>8);
		out.put(c>>16);
		out.put(c>>24);
	}
	
	out.close();
}

void writeAsmOutput(string file)
{
	ofstream out ((file+".s").c_str(), ios_base::out);

	if(!out)
	{
		cout<<"Fail opening output file."<<endl;
		exit(1);
	}

	out<<".section .rodata"<<endl;
	out<<".align 2"<<endl;
	out<<".global "<<file<<"Model"<<endl;
	out<<" "<<file<<"Model:"<<endl;

	int asmWordCount = 0;
	for(int i = 0; i < result.size(); i++)
	{
		int c = result[i];
		if(asmWordCount == 0)
			out<<endl<<"	.word ";
		else
			out<<",";
		out<<"0x";
		out<<setbase(16)<<c;
		
		asmWordCount++;
		asmWordCount %= 8;
	}
	
	out<<endl;
	out.close();

	ofstream header ((file+"Model.h").c_str(), ios_base::out);
	header<<"extern const u8 "<<file<<"Model[];"<<endl;
	header.close();
}


int main(int argc, const char** argv)
{
	if(argc != 4)
	{
		cout<<"Usage: obj2nds [-b|-s] infile.obj outfile "<<endl;
		cout<<"If -s is set, it generates outfile.s and outfile.h"<<endl;
		cout<<"If -b is set, it generates outfile as a binary file"<<endl;
		return 1;
	}
	
	cout<<"obj2nds alpha 0.1, by Dirbaio"<<endl;	
	
	ifstream in ( argv[2] , ifstream::in );

	if(!in)
	{
		cout<<"Fail opening input file."<<endl;
		return 1;
	}
	
	string optsfile = argv[2];
	optsfile += "2nds";
	ifstream opts (optsfile.c_str(), ifstream::in );
	if(opts)
	{
		string cmd;
		while(opts>>cmd)
		{
			if(cmd == "normals")
			{
				string val;
				opts>>val;
				writeNormals = parseBool(val);
			}
			else if(cmd == "recalc-normals")
			{
				string val;
				opts>>val;
				recalcNormals = parseBool(val);
			}
			else if(cmd == "texcoords")
			{
				string val;
				opts>>val;
				writeTexCoords = parseBool(val);
			}
			else if(cmd == "scale")
			{
				opts>>scale;
			}
			else if(cmd == "texcoord-scale")
			{
				opts>>texScale;
			}
			else cout<<"Ignoring bad option: "<<cmd<<endl;
		}
	}
	else
	{
		cout<<"Could not find options file. Using default options."<<endl;
	}
	

	
	string s;
	writeInt(0x12345678); //Size of the display list. Will fill later :)
	listSize = 0; //We don't want to count the size field.
	
	int oldMode = -1;
	
	writeColorCmd(255, 255, 255);
	
	while(getline(in, s))
	{
		stringstream ss(s);
		if(ss.peek() == '#') continue; //Comments have to be ignored.
		
		string cmd;
		ss>>cmd;
		
		if(cmd == "v")
		{
			vec3 v;
			ss>>v.x>>v.y>>v.z;
			vtxs.push_back(v);
		}
		else if(cmd == "vt")
		{
			vec3 t;
			ss>>t.x>>t.y>>t.z;
			texs.push_back(t);
		}
		else if(cmd == "vn")
		{
			vec3 n;
			ss>>n.x>>n.y>>n.z;
			n = normalize(n);
			norms.push_back(n);
		}
		else if(cmd == "f")
		{
			writeFace(ss, oldMode);
				
//			writeInt(END_VTXS); //Not needed???
		}
		
	}
	
	result[0] = listSize;
	
	in.close();
	
	string outtype = argv[1];
	if(outtype == "-s")
		writeAsmOutput(argv[3]);
	else
		writeBinaryOutput(argv[3]);

	cout<<"Done: "<<triCount<<" triangles, "<<quadCount<<" quads"<<endl;
	
	return 0;
}