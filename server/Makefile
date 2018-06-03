CC = g++

IDIR = "./include"
ODIR = "./src"
OUT_DIR = "./bin"

CFLAGS = -I $(IDIR) -std=gnu++11 -c `sdl-config --cflags`
OBJ = $(ODIR)/Logger.o $(ODIR)/UdpNetworkService.o $(ODIR)/ServerConfiguration.o $(ODIR)/ServiceState.o $(ODIR)/CommandProcessor/CommandProcessor.o $(ODIR)/CommandProcessor/CommandTransaction.o $(ODIR)/Core/GameState.o $(ODIR)/Core/User.o $(ODIR)/Core/Registration.o $(ODIR)/Core/Notification.o $(ODIR)/CommandProcessor/TransactionBuilder.o $(ODIR)/CommandProcessor/CommandExecutor/CommandExecutor.o $(ODIR)/CommandProcessor/CommandExecutor/UserCommandExecutor.o $(ODIR)/CommandProcessor/CommandExecutor/NotificationCommandExecutor.o  $(ODIR)/main.o

all: rpg_net_make clean_objs

rpg_net_make: $(OBJ)
	$(CC) -o $(OUT_DIR)/rpg_net $(OBJ) -lSDL -lSDL_net -l pthread $(CLFAGS)

$(ODIR)%.o: ./src/%.cpp 
	$(CC) -c -o $@ $< $(CFLAGS)

clean_objs:
	rm $(ODIR)/*.o $(ODIR)/CommandProcessor/*.o $(ODIR)/CommandProcessor/CommandExecutor/*.o $(ODIR)/Core/*.o

clean:
	rm $(OUT_DIR)/rpg_net $(ODIR)/*.o