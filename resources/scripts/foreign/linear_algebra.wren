foreign class Vec3 {
	x {this[0]}
	y {this[1]}
	z {this[2]}
	
	x=(value) {this[0]=value}
	y=(value) {this[1]=value}
	z=(value) {this[2]=value}
	
	length_sqr {dot(self, self)}
	length {length_sqr.sqrt}
	
	foreign [k]
	foreign [k]=(x)

	test (){
		return 1
	}

	construct set(x, y, z){}
	
	- { 
		return this * -1
	}
	
	+ (other) {
		return Vec3.set(x + other.x, y + other.y, z + other.z)
	}
	
	- (other) {
		return Vec3.set(x - other.x, y - other.y, z - other.z)
	}
	
	* (other) {
		return Vec3.set(x * other, y * other, z * other)
	}
	
	/ (other) {
		return Vec3.set(x / other, y / other, z / other)
	}
	
	norm() {
		return Vec3.dot(this, this).sqrt
	}
	
	normalized() {
		return this / norm()
	}
	
	static dot(v1, v2) {
		return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z
	}
	
	static cross(v1, v2) {
		return set(
			v1.y*v2.z - v2.y*v1.z,
			v2.x*v1.z - v1.x*v2.z,
			v1.x*v2.y - v2.x*v1.y
		)
	}
	
	static angle(v1, v2) {
		return (dot(v1, v2) / (dot(v1, v1) * dot(v2, v2)).sqrt).acos
	}
	
	toString {
		return "[%(x); %(y); %(z)]"
	}
}

foreign class Transform {
	foreign [i, j]
	foreign [i, j]=(a)

	static identity {set(Vec3.set(1, 0, 0), Vec3.set(0, 1, 0), Vec3.set(0, 0, 1), Vec3.set(0, 0, 0))}

	construct set(v1, v2, v3, t){}
	
	toString {
		var res = ""
		res = res + "/%(this[0, 0]); %(this[0, 1]); %(this[0, 2]); %(this[0, 3])\\\n"
		res = res + "|%(this[1, 0]); %(this[1, 1]); %(this[1, 2]); %(this[1, 3])|\n"
		res = res + "|%(this[2, 0]); %(this[2, 1]); %(this[2, 2]); %(this[2, 3])|\n"
		res = res +"\\%(this[3, 0]); %(this[3, 1]); %(this[3, 2]); %(this[3, 3])/\n"
		return res
	}
	
	foreign transform_point(v)
	foreign transform_direction(v)
	foreign mul(m)
	foreign scale(m)
	foreign + (m)
	-(m) { 
		return this + other.scale(-1)
	}
	
	* (other) {
		if (other is Num){
			return scale(other)
		} else if (other is Vec3){
			return transform_point(other)
		} else if (other is Transform){
			return mul(other)
		}
		return this
	}
}
