import { createFileRoute, Link } from "@tanstack/react-router";
import { useState } from "react";
import { Mail, Lock, ArrowRight } from "lucide-react";

import { supabase } from "@/integrations/supabase/client";
import logoAsset from "@/assets/sentinel-logo.png.asset.json";

export const Route = createFileRoute("/forgot-password")({
  head: () => ({
    meta: [
      { title: "Password Recovery — SentinelAI-X" },
      {
        name: "description",
        content:
          "Recover access to your SentinelAI-X laboratory security account with a secure email verification link.",
      },
      { property: "og:title", content: "Password Recovery — SentinelAI-X" },
      {
        property: "og:description",
        content: "Secure password recovery for authorized SentinelAI-X accounts.",
      },
    ],
  }),
  component: ForgotPassword,
});

function ForgotPassword() {
  const [email, setEmail] = useState("");
  const [sent, setSent] = useState(false);
  const [loading, setLoading] = useState(false);

  async function onSubmit(e: React.FormEvent) {
    e.preventDefault();
    setLoading(true);
    await supabase.auth.resetPasswordForEmail(email.trim(), {
      redirectTo: `${window.location.origin}/reset-password`,
    });
    setLoading(false);
    setSent(true);
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
            Password Recovery
          </p>
          <div className="mx-auto mt-3 h-[3px] w-16 rounded-full bg-brand-line" />
          <h1 className="mt-4 text-center text-4xl font-bold tracking-tight text-brand-navy">
            Verify Email
          </h1>

          {sent ? (
            <p className="mt-4 text-center text-base text-brand-muted">
              If this email belongs to an authorized account, a verification link
              has been sent. Open it to reset your password.
            </p>
          ) : (
            <form onSubmit={onSubmit} className="mt-8 space-y-5">
              <div>
                <label
                  htmlFor="recovery-email"
                  className="block text-[15px] font-bold text-brand-navy"
                >
                  Email Address
                </label>
                <div className="field mt-2">
                  <Mail className="h-5 w-5 text-brand-icon" />
                  <input
                    id="recovery-email"
                    type="email"
                    required
                    maxLength={255}
                    value={email}
                    onChange={(e) => setEmail(e.target.value)}
                    placeholder="Enter your email"
                    className="field-input"
                  />
                </div>
              </div>
              <button type="submit" disabled={loading} className="btn-brand">
                <span>{loading ? "Sending…" : "Send Verification"}</span>
                <ArrowRight className="h-6 w-6" strokeWidth={3} />
              </button>
            </form>
          )}

          <div className="mt-6 text-center">
            <Link
              to="/"
              className="text-[15px] font-semibold text-brand-blue hover:underline"
            >
              Back to Sign In
            </Link>
          </div>
        </div>
      </div>
    </main>
  );
}
