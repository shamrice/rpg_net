CC = g++

IDIR = "./include"
ODIR = "./src"
OUT_DIR = "./bin"

CFLAGS = -I $(IDIR) -std=gnu++11 -c `sdl-config --cflags`
OBJ = $(ODIR)/Engine.o $(ODIR)/UdpClientService.o $(ODIR)/ClientConfiguration.o $(ODIR)/User.o $(ODIR)/Logger.o $(ODIR)/main.o

all: rpg_net_client_make clean_objs

rpg_net_client_make: $(OBJ)
	$(CC) -o $(OUT_DIR)/rpg_net_client $(OBJ) -lncurses -lSDL -lSDL_net $(CLFAGS)

$(ODIR)%.o: ./src/%.cpp 
	$(CC) -c -o $@ $< $(CFLAGS)

clean_objs:
	rm $(ODIR)/*.o 

clean:
	rm $(OUT_DIR)/rpg_net_client $(ODIR)/*.o