using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;

namespace NEWorldLauncher
{
    public class Game
    {
        public string Name { get; set; }
        public Process process { get; set; }

        public Game(string name)
        {
            Name = name;
            var gamePath = Path.GetFullPath(Path.Combine(Def.gamesPath, Name));
            process = new Process();
            process.StartInfo.FileName = Path.Combine(gamePath, Def.exeName);
            process.StartInfo.WorkingDirectory = gamePath;
        }

        public void Launch(bool debug, string command)
        {
            process.StartInfo.Arguments = command;
            if (debug)
            {
                process.StartInfo.UseShellExecute = false;
                process.StartInfo.RedirectStandardOutput = true;
                process.EnableRaisingEvents = true;
            }
            process.Start();
        }

        public static string[] GetGameNames()
        {
            if (!Directory.Exists(Def.gamesPath))
                Directory.CreateDirectory(Def.gamesPath);
            string[] directories = Directory.GetDirectories(Def.gamesPath);
            var names = new List<string>();
            foreach (var i in directories)
                if (File.Exists(Path.Combine(i, Def.exeName)))
                    names.Add(Path.GetFileName(i));
            return names.ToArray();
        }
    }
}
