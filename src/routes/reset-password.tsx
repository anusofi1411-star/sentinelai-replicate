import { createFileRoute, useNavigate } from "@tanstack/react-router";
import { useState } from "react";
import { Lock, ArrowRight } from "lucide-react";

import { supabase } from "@/integrations/supabase/client";
import logoAsset from "@/assets/sentinel-logo.png.asset.json";

export const Route = createFileRoute("/reset-password")({
  head: () => ({
    meta: [
      { title: "Reset Password — SentinelAI-X" },
      {
        name: "description",
        content: "Set a new password for your SentinelAI-X laboratory security account.",
      },
      { property: "og:title", content: "Reset Password — SentinelAI-X" },
      {
        property: "og:description",
        content: "Set a new password for authorized SentinelAI-X access.",
      },
    ],
  }),
  component: ResetPassword,
});

function ResetPassword() {
  const navigate = useNavigate();
  const [password, setPassword] = useState("");
  const [confirm, setConfirm] = useState("");
  const [message, setMessage] = useState<string | null>(null);
  const [loading, setLoading] = useState(false);

  async function onSubmit(e: React.FormEvent) {
    e.preventDefault();
    setMessage(null);
    if (password.length < 8) {
      setMessage("Password must be at least 8 characters.");
      return;
    }
    if (password !== confirm) {
      setMessage("Passwords do not match.");
      return;
    }
    setLoading(true);
    const { error } = await supabase.auth.updateUser({ password });
    setLoading(false);
    if (error) {
      setMessage("Reset link is invalid or expired. Request a new one.");
      return;
    }
    navigate({ to: "/dashboard" });
  }

  return (
    <main className="relative flex min-h-screen items-center justify-center overflow-hidden bg-brand-canvas px-6 py-10">
      <div className="brand-glow-blue" aria-hidden="true" />
      <div className="brand-glow-pink" aria-hidden="true" />
      <div className="relative w-full max-w-[560px]">
        <img
          src={logoAsset.url}
          alt="SentinelAI-X"
          className="mx-auto mb-8 w-full max-w-[420px]"
        />
        <div className="login-card px-7 py-10 sm:px-10 sm:py-12">
          <div className="flex justify-center">
            <div className="shield-badge">
              <Lock className="h-7 w-7 text-white" strokeWidth={3} />
            </div>
          </div>
          <p className="mt-6 text-center text-lg font-bold uppercase tracking-[0.28em] text-gradient-brand">
            Secure Reset
          </p>
          <div className="mx-auto mt-3 h-[3px] w-16 rounded-full bg-brand-line" />
          <h1 className="mt-4 text-center text-4xl font-bold tracking-tight text-brand-navy">
            New Password
          </h1>

          <form onSubmit={onSubmit} className="mt-8 space-y-5">
            <div>
              <label
                htmlFor="new-password"
                className="block text-[15px] font-bold text-brand-navy"
              >
                New Password
              </label>
              <div className="field mt-2">
                <Lock className="h-5 w-5 text-brand-icon" />
                <input
                  id="new-password"
                  type="password"
                  required
                  maxLength={128}
                  value={password}
                  onChange={(e) => setPassword(e.target.value)}
                  placeholder="Enter new password"
                  className="field-input"
                />
              </div>
            </div>
            <div>
              <label
                htmlFor="confirm-password"
                className="block text-[15px] font-bold text-brand-navy"
              >
                Confirm Password
              </label>
              <div className="field mt-2">
                <Lock className="h-5 w-5 text-brand-icon" />
                <input
                  id="confirm-password"
                  type="password"
                  required
                  maxLength={128}
                  value={confirm}
                  onChange={(e) => setConfirm(e.target.value)}
                  placeholder="Confirm new password"
                  className="field-input"
                />
              </div>
            </div>
            {message && (
              <p className="text-center text-sm font-semibold text-destructive">
                {message}
              </p>
            )}
            <button type="submit" disabled={loading} className="btn-brand">
              <span>{loading ? "Updating…" : "Update Password"}</span>
              <ArrowRight className="h-6 w-6" strokeWidth={3} />
            </button>
          </form>
        </div>
      </div>
    </main>
  );
}
