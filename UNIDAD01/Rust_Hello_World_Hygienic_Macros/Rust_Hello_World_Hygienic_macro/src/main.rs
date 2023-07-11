
macro_rules! f{
($a:expr,$b:expr)=>{
    {
      $a*$z
    }
  }
}

fn main() {
 let (a, b, z) = (2, 5, 3);
 println!("Multiplication Macro in Rust = {}", f!(a, b));
}

 

