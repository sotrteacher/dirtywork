//REF: Pag. 32 de "The Rust Programming Language"
// (62/554) Steve Klabnik, Carol Nichols, Rust Community - 
// The Rust Programming Language-No Starch Press (2018).pdf
//Error debido a que la variable x es immutable.
fn main() {
 let x = 5;
 println!("The value of x is {}",x);
 x = 6;
 println!("The value of x is {}",x);
}

//As\'i no hay error debido a que la variable x
//ahora si es mutable.
//fn main() {
// let mut x = 5;
// println!("The value of x is {}",x);
// x = 6;
// println!("The value of x is {}",x);
//}
//
