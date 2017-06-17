using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using CollectorsFrontEnd.InterfaceModels;

namespace CollectorsFrontEnd.StoreFrontSupport
{
    public partial class ServerInterface
    {
        // The static singleton is exposed via the 3 public properties Server, Card, and Collection
        // The Singleton's constructor ensures that the private static member SCI is initiated prior
        // to use. 3 Different public exposures for groups of functions gives the flat interface 
        // structure that is more readable.
        private static ServerInterface Singleton;

        // SCI is the Managed C++ interface. It is a flat interface, meaning ALL available server
        // functions are here. It acts as a BLOB class for ease of use due to the nature of 
        // managed <-> unmanaged communications, despite being an 'anti-pattern'.
        private static ServerClientInterface SCI;

        public static ServerIFace Server
        {
            get
            {
                if (Singleton == null) { Singleton = new ServerInterface(); }
                return Singleton._Server;
            }
        }
        public static CardIFace Card
        {
            get
            {
                if (Singleton == null) { Singleton = new ServerInterface(); }
                return Singleton._Card;
            }
        }
        public static CollectionIFace Collection
        {
            get
            {
                if (Singleton == null) { Singleton = new ServerInterface(); }
                return Singleton._Collection;
            }
        }

        private ServerIFace _Server;
        private CardIFace _Card;
        private CollectionIFace _Collection;

        /// <summary>
        /// The Constructor initilizes all the objects in the correct order.
        /// </summary>
        private ServerInterface()
        {
            SCI = new ServerClientInterface();

            _Server = new ServerIFace();
            _Card = new CardIFace();
            _Collection = new CollectionIFace();
        }
    }
}
