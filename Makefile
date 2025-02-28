default: Tren

DEPS=AGL3Window.cpp AGL3Window.hpp AGL3Drawable.hpp utils.hpp Map.hpp Map.cpp Sphere.hpp

# Reguła jak zbudować plik wykonywalny z pliku .cpp
%: %.cpp $(DEPS)
	g++ -I. $< -o $@ AGL3Window.cpp Map.cpp -lepoxy  -lglfw 

# A tu dodatkowy target, którego wywołanie "make clean" czyści
clean:
	rm a.out *.o *~ Tren
