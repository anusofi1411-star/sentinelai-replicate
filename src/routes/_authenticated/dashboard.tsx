import { createFileRoute, useNavigate } from "@tanstack/react-router";
import { useQueryClient } from "@tanstack/react-query";
import { ShieldCheck, LogOut } from "lucide-react";

import { supabase } from "@/integrations/supabase/client";
import logoAsset from "@/assets/sentinel-logo.png.asset.json";
import labAsset from "@/assets/lab-network.png.asset.json";

export const Route = createFileRoute("/_authenticated/dashboard")({
  head: () => ({
    meta: [
      { title: "Security Dashboard — SentinelAI-X" },
      {
        name: "description",
        content:
          "Monitor LAB 01 to LAB 04 laboratory security nodes from the SentinelAI-X dashboard.",
      },
      { property: "og:title", content: "Security Dashboard — SentinelAI-X" },
      {
        property: "og:description",
        content: "Live laboratory security monitoring for authorized SentinelAI-X users.",
      },
    ],
  }),
  component: Dashboard,
});

function Dashboard() {
  const navigate = useNavigate();
  const queryClient = useQueryClient();
  const { user } = Route.useRouteContext();

  async function signOut() {
    await queryClient.cancelQueries();
    queryClient.clear();
    await supabase.auth.signOut();
    navigate({ to: "/", replace: true });
  }

  return (
    <main className="relative min-h-screen overflow-hidden bg-brand-canvas px-6 py-10">
      <div className="brand-glow-blue" aria-hidden="true" />
      <div className="brand-glow-pink" aria-hidden="true" />
      <div className="relative mx-auto w-full max-w-5xl">
        <div className="flex flex-wrap items-center justify-between gap-4">
          <img src={logoAsset.url} alt="SentinelAI-X" className="w-full max-w-[340px]" />
          <button onClick={signOut} className="btn-brand w-auto px-6 py-3 text-base">
            <span>Sign Out</span>
            <LogOut className="h-5 w-5" strokeWidth={3} />
          </button>
        </div>

        <div className="login-card mt-8 px-7 py-10 sm:px-10">
          <div className="flex items-center gap-3">
            <ShieldCheck className="h-7 w-7 text-brand-blue" />
            <h1 className="text-3xl font-bold text-brand-navy">Security Dashboard</h1>
          </div>
          <p className="mt-2 text-base text-brand-muted">
            Signed in as {user.email}
          </p>
          <div className="mt-6 grid grid-cols-2 gap-4 sm:grid-cols-4">
            {["LAB 01", "LAB 02", "LAB 03", "LAB 04"].map((lab) => (
              <div key={lab} className="rounded-2xl border border-brand-border bg-white/70 p-4 text-center">
                <p className="text-sm font-bold tracking-widest text-brand-blue">{lab}</p>
                <p className="mt-1 text-sm text-brand-muted">Secure</p>
              </div>
            ))}
          </div>
          <img
            src={labAsset.url}
            alt="Distributed laboratory network overview"
            className="mt-8 w-full"
          />
        </div>
      </div>
    </main>
  );
}
